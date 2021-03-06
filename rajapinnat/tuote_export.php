<?php

	// Kutsutaanko CLI:st�
	$php_cli = FALSE;

	if (php_sapi_name() == 'cli') {
		$php_cli = TRUE;
	}

	date_default_timezone_set('Europe/Helsinki');

	// Kutsutaanko CLI:st�
	if (!$php_cli) {
		die ("T�t� scripti� voi ajaa vain komentorivilt�!");
	}

	$pupe_root_polku = dirname(dirname(__FILE__));

	require ("{$pupe_root_polku}/inc/connect.inc");
	require ("{$pupe_root_polku}/inc/functions.inc");

	$lock_params = array(
		"locktime" => 5400,
		"lockfile" => '##tuote-export-flock.lock',
	);

	// Sallitaan vain yksi instanssi t�st� skriptist� kerrallaan
	pupesoft_flock($lock_params);

	require ("{$pupe_root_polku}/rajapinnat/magento_client.php");

	// Laitetaan unlimited execution time
	ini_set("max_execution_time", 0);

	if (trim($argv[1]) != '') {
		$yhtio = mysql_real_escape_string($argv[1]);
		$yhtiorow = hae_yhtion_parametrit($yhtio);
		$kukarow = hae_kukarow('admin', $yhtio);

		if ($kukarow === null) {
			die ("\n");
		}
	}
	else {
		die ("Et antanut yhti�t�.\n");
	}

	$verkkokauppatyyppi = isset($argv[2]) ? trim($argv[2]) : "";

	if ($verkkokauppatyyppi != "magento" and $verkkokauppatyyppi != "anvia") {
		die ("Et antanut verkkokaupan tyyppi�.\n");
	}

	if (isset($verkkokauppatyyppi) and $verkkokauppatyyppi == "magento") {

		// Varmistetaan, ett� kaikki muuttujat on kunnossa
		if (empty($magento_api_ana_url) or empty($magento_api_ana_usr) or empty($magento_api_ana_pas) or empty($magento_tax_class_id)) {
			echo "Magento parametrit puuttuu, p�ivityst� ei voida ajaa.";
			exit;
		}
	}

	$ajetaanko_kaikki = (isset($argv[3]) and trim($argv[3]) != '') ? "YES" : "NO";

	if (isset($verkkokauppa_saldo_varasto) and !is_array($verkkokauppa_saldo_varasto)) {
		echo "verkkokauppa_saldo_varasto pit�� olla array!";
		exit;
	}

	// Haetaan timestamp
	$datetime_checkpoint_res = t_avainsana("TUOTE_EXP_CRON");

	if (mysql_num_rows($datetime_checkpoint_res) != 1) {
		exit("VIRHE: Timestamp ei l�ydy avainsanoista!\n");
	}

	$datetime_checkpoint_row = mysql_fetch_assoc($datetime_checkpoint_res);
	$datetime_checkpoint = $datetime_checkpoint_row['selite']; // Mik� tilanne on jo k�sitelty
	$datetime_checkpoint_uusi = date('Y-m-d H:i:s'); // Timestamp nyt

	// alustetaan arrayt
	$dnstuote = $dnsryhma = $dnstuoteryhma = $dnstock = $dnsasiakas = $dnshinnasto = $dnslajitelma = $kaikki_tuotteet = array();

	if ($ajetaanko_kaikki == "NO") {
		$muutoslisa = "AND (tuote.muutospvm >= '{$datetime_checkpoint}'
							OR ta_nimitys_se.muutospvm >= '{$datetime_checkpoint}'
							OR ta_nimitys_en.muutospvm >= '{$datetime_checkpoint}'
							)";
	}
	else {
		$muutoslisa = "";
	}

	echo date("d.m.Y @ G:i:s")." - Aloitetaan tuote-export.\n";
	echo date("d.m.Y @ G:i:s")." - Haetaan tuotetiedot.\n";

	// Haetaan pupesta tuotteen tiedot
	$query = "	SELECT tuote.tuoteno,
				tuote.nimitys,
				tuote.lyhytkuvaus,
				tuote.myyntihinta,
				tuote.yksikko,
				tuote.kuvaus,
				tuote.myymalahinta,
				tuote.kuluprosentti,
				tuote.eankoodi,
				tuote.osasto,
				tuote.try,
				tuote.alv,
				tuote.nakyvyys,
				tuote.tuotemassa,
				tuote.tunnus,
				tuote.mallitarkenne campaign_code,
				tuote.malli target,
				tuote.leimahduspiste onsale,
				ta_nimitys_se.selite nimi_swe,
				ta_nimitys_en.selite nimi_eng,
				try_fi.selitetark try_nimi
				FROM tuote
				LEFT JOIN avainsana as try_fi ON (try_fi.yhtio = tuote.yhtio and try_fi.selite = tuote.try and try_fi.laji = 'try' and try_fi.kieli = 'fi')
				LEFT JOIN tuotteen_avainsanat as ta_nimitys_se on tuote.yhtio = ta_nimitys_se.yhtio and tuote.tuoteno = ta_nimitys_se.tuoteno and ta_nimitys_se.laji = 'nimitys' and ta_nimitys_se.kieli = 'se'
				LEFT JOIN tuotteen_avainsanat as ta_nimitys_en on tuote.yhtio = ta_nimitys_en.yhtio and tuote.tuoteno = ta_nimitys_en.tuoteno and ta_nimitys_en.laji = 'nimitys' and ta_nimitys_en.kieli = 'en'
				WHERE tuote.yhtio = '{$kukarow["yhtio"]}'
				AND tuote.status != 'P'
				AND tuote.tuotetyyppi NOT in ('A','B')
				AND tuote.tuoteno != ''
				AND tuote.nakyvyys != ''
				$muutoslisa
	 			ORDER BY tuote.tuoteno";
	$res = pupe_query($query);

	// Py�r�ytet��n muuttuneet tuotteet l�pi
	while ($row = mysql_fetch_array($res)) {

		// Jos yhti�n hinnat eiv�t sis�ll� alv:t�
		if ($yhtiorow["alv_kasittely"] != "" and $verkkokauppatyyppi != 'magento') {
			$myyntihinta					= hintapyoristys($row["myyntihinta"] * (1+($row["alv"]/100)));
			$myyntihinta_veroton 			= $row["myyntihinta"];

			$myymalahinta					= hintapyoristys($row["myymalahinta"] * (1+($row["alv"]/100)));
			$myymalahinta_veroton 			= $row["myymalahinta"];
		}
		else {
			$myyntihinta					= $row["myyntihinta"];
			$myyntihinta_veroton 			= hintapyoristys($row["myyntihinta"] / (1+($row["alv"]/100)));

			$myymalahinta					= $row["myymalahinta"];
			$myymalahinta_veroton 			= hintapyoristys($row["myymalahinta"] / (1+($row["alv"]/100)));
		}

		$dnstuote[] = array('tuoteno'				=> $row["tuoteno"],
							'nimi'					=> $row["nimitys"],
							'kuvaus'				=> $row["kuvaus"],
							'lyhytkuvaus'			=> $row["lyhytkuvaus"],
							'yksikko'				=> $row["yksikko"],
							'tuotemassa'			=> $row["tuotemassa"],
							'myyntihinta'			=> $myyntihinta,
							'myyntihinta_veroton'	=> $myyntihinta_veroton,
							'myymalahinta'			=> $myymalahinta,
							'myymalahinta_veroton'	=> $myymalahinta_veroton,
							'kuluprosentti'			=> $row['kuluprosentti'],
							'ean'					=> $row["eankoodi"],
							'osasto'				=> $row["osasto"],
							'try'					=> $row["try"],
							'try_nimi'				=> $row["try_nimi"],
							'alv'					=> $row["alv"],
							'nakyvyys'				=> $row["nakyvyys"],
							'nimi_swe'				=> $row["nimi_swe"],
							'nimi_eng'				=> $row["nimi_eng"],
							'campaign_code'			=> $row["campaign_code"],
							'target'				=> $row["target"],
							'onsale'				=> $row["onsale"],
							'tunnus'				=> $row['tunnus'],
							);
	}

	// Magentoa varten pit�� hakea kaikki tuotteet, jotta voidaan poistaa ne jota ei ole olemassa
	if ($verkkokauppatyyppi == 'magento') {

		echo date("d.m.Y @ G:i:s")." - Haetaan poistettavat tuotteet.\n";

		// Haetaan pupesta kaikki tuotteet (ja configurable-tuotteet), jotka pit�� olla Magentossa
		$query = "	SELECT DISTINCT tuotteen_avainsanat.selite configurable_tuoteno, tuote.tuoteno
					FROM tuotteen_avainsanat
					JOIN tuote ON (tuote.yhtio = tuotteen_avainsanat.yhtio
					AND tuote.tuoteno = tuotteen_avainsanat.tuoteno
					AND tuote.status != 'P'
					AND tuote.tuotetyyppi NOT IN ('A','B')
					AND tuote.tuoteno != ''
					AND tuote.nakyvyys != '')
					WHERE tuotteen_avainsanat.yhtio = '{$kukarow["yhtio"]}'
					AND tuotteen_avainsanat.laji = 'parametri_variaatio'
					AND trim(tuotteen_avainsanat.selite) != ''";
		$res = pupe_query($query);

		// Kaikki tuotenumerot arrayseen
		while ($row = mysql_fetch_array($res)) {
			$kaikki_tuotteet[] = $row['tuoteno'];
			$kaikki_tuotteet[] = $row['configurable_tuoteno'];
		}

		$kaikki_tuotteet = array_unique($kaikki_tuotteet);
	}

	echo date("d.m.Y @ G:i:s")." - Haetaan saldot.\n";

	if ($ajetaanko_kaikki == "NO") {
		$muutoslisa1 = "AND tapahtuma.laadittu >= '{$datetime_checkpoint}'";
		$muutoslisa2 = "AND tilausrivi.laadittu >= '{$datetime_checkpoint}'";
		$muutoslisa3 = "AND tuote.muutospvm >= '{$datetime_checkpoint}'";
	}
	else {
		$muutoslisa1 = "";
		$muutoslisa2 = "";
		$muutoslisa3 = "";
	}

	// Haetaan saldot tuotteille, joille on tehty tunnin sis�ll� tilausrivi tai tapahtuma
	$query =  "(SELECT tapahtuma.tuoteno,
				tuote.eankoodi
				FROM tapahtuma
				JOIN tuote ON (tuote.yhtio = tapahtuma.yhtio
					AND tuote.tuoteno = tapahtuma.tuoteno
					AND tuote.status != 'P'
					AND tuote.tuotetyyppi NOT in ('A','B')
					AND tuote.tuoteno != ''
					AND tuote.nakyvyys != '')
				WHERE tapahtuma.yhtio = '{$kukarow["yhtio"]}'
				$muutoslisa1)

				UNION

				(SELECT tilausrivi.tuoteno,
				tuote.eankoodi
				FROM tilausrivi
				JOIN tuote ON (tuote.yhtio = tilausrivi.yhtio
					AND tuote.tuoteno = tilausrivi.tuoteno
					AND tuote.status != 'P'
					AND tuote.tuotetyyppi NOT in ('A','B')
					AND tuote.tuoteno != ''
					AND tuote.nakyvyys != '')
				WHERE tilausrivi.yhtio = '{$kukarow["yhtio"]}'
				$muutoslisa2)

				UNION

				(SELECT tuote.tuoteno,
				tuote.eankoodi
				FROM tuote
				WHERE tuote.yhtio = '{$kukarow["yhtio"]}'
				AND tuote.status != 'P'
				AND tuote.tuotetyyppi NOT in ('A','B')
				AND tuote.tuoteno != ''
				AND tuote.nakyvyys != ''
				$muutoslisa3)

				ORDER BY 1";
	$result = pupe_query($query);

	while ($row = mysql_fetch_assoc($result)) {
		list(,,$myytavissa) = saldo_myytavissa($row["tuoteno"], '', $verkkokauppa_saldo_varasto);
		$dnstock[] = array(	'tuoteno'		=> $row["tuoteno"],
							'ean'			=> $row["eankoodi"],
							'myytavissa'	=> $myytavissa,
							);
	}

	if ($ajetaanko_kaikki == "NO") {
		$muutoslisa = "AND (try_fi.muutospvm >= '{$datetime_checkpoint}'
			OR try_se.muutospvm >= '{$datetime_checkpoint}'
			OR try_en.muutospvm >= '{$datetime_checkpoint}'
			OR osasto_fi.muutospvm >= '{$datetime_checkpoint}'
			OR osasto_se.muutospvm >= '{$datetime_checkpoint}'
			OR osasto_en.muutospvm >= '{$datetime_checkpoint}')";
	}
	else {
		$muutoslisa = "";
	}

	echo date("d.m.Y @ G:i:s")." - Haetaan osastot/tuoteryhm�t.\n";

	// Haetaan kaikki TRY ja OSASTO:t, niiden muutokset.
	$query = "	SELECT DISTINCT	tuote.osasto,
				tuote.try,
				try_fi.selitetark try_fi_nimi,
				try_se.selitetark try_se_nimi,
				try_en.selitetark try_en_nimi,
				osasto_fi.selitetark osasto_fi_nimi,
				osasto_se.selitetark osasto_se_nimi,
				osasto_en.selitetark osasto_en_nimi
				FROM tuote
				LEFT JOIN avainsana as try_fi ON (try_fi.yhtio = tuote.yhtio and try_fi.selite = tuote.try and try_fi.laji = 'try' and try_fi.kieli = 'fi')
				LEFT JOIN avainsana as try_se ON (try_se.yhtio = tuote.yhtio and try_se.selite = tuote.try and try_se.laji = 'try' and try_se.kieli = 'se')
				LEFT JOIN avainsana as try_en ON (try_en.yhtio = tuote.yhtio and try_en.selite = tuote.try and try_en.laji = 'try' and try_en.kieli = 'en')
				LEFT JOIN avainsana as osasto_fi ON (osasto_fi.yhtio = tuote.yhtio and osasto_fi.selite = tuote.osasto and osasto_fi.laji = 'osasto' and osasto_fi.kieli = 'fi')
				LEFT JOIN avainsana as osasto_se ON (osasto_se.yhtio = tuote.yhtio and osasto_se.selite = tuote.osasto and osasto_se.laji = 'osasto' and osasto_se.kieli = 'se')
				LEFT JOIN avainsana as osasto_en ON (osasto_en.yhtio = tuote.yhtio and osasto_en.selite = tuote.osasto and osasto_en.laji = 'osasto' and osasto_en.kieli = 'en')
				WHERE tuote.yhtio = '{$kukarow["yhtio"]}'
				AND tuote.status != 'P'
				AND tuote.tuotetyyppi NOT in ('A','B')
				AND tuote.tuoteno != ''
				AND tuote.nakyvyys != ''
				$muutoslisa
				ORDER BY 1, 2";
	$try_result = pupe_query($query);

	while ($row = mysql_fetch_assoc($try_result)) {

		// Osasto/tuoteryhm� array
		$dnsryhma[$row["osasto"]][$row["try"]] = array(	'osasto'	=> $row["osasto"],
														'try'		=> $row["try"],
														'osasto_fi'	=> $row["osasto_fi_nimi"],
														'try_fi'	=> $row["try_fi_nimi"],
														'osasto_se'	=> $row["osasto_se_nimi"],
														'try_se'	=> $row["try_se_nimi"],
														'osasto_en' => $row["osasto_en_nimi"],
														'try_en'	=> $row["try_en_nimi"],
														);

		// Ker�t��n my�s pelk�t tuotenumerot Magentoa varten
		$dnstuoteryhma[$row["try"]] = array(	'try'		=> $row["try"],
												'try_fi'	=> $row["try_fi_nimi"],
												'try_se'	=> $row["try_se_nimi"],
												'try_en'	=> $row["try_en_nimi"],
												);
	}

	if ($ajetaanko_kaikki == "NO") {
		$muutoslisa = "AND asiakas.muutospvm >= '{$datetime_checkpoint}'";
	}
	else {
		$muutoslisa = "";
	}

	echo date("d.m.Y @ G:i:s")." - Haetaan asiakkaat.\n";

	// Haetaan kaikki asiakkaat
	$query = "	SELECT asiakas.nimi,
				asiakas.osoite,
				asiakas.postino,
				asiakas.postitp,
				asiakas.email
				FROM asiakas
				WHERE asiakas.yhtio = '{$kukarow["yhtio"]}'
				AND asiakas.laji != 'P'
				$muutoslisa";
	$res = pupe_query($query);

	// py�r�ytet��n asiakkaat l�pi
	while ($row = mysql_fetch_array($res)) {
		$dnsasiakas[] = array(	'nimi'		=> $row["nimi"],
								'osoite'	=> $row["osoite"],
								'postino'	=> $row["postino"],
								'postitp'	=> $row["postitp"],
								'email'		=> $row["email"],
								);
	}

	if ($ajetaanko_kaikki == "NO") {
		$muutoslisa = "AND hinnasto.muutospvm >= '{$datetime_checkpoint}'";
	}
	else {
		$muutoslisa = "";
	}

	echo date("d.m.Y @ G:i:s")." - Haetaan hinnastot.\n";

	// Haetaan kaikki hinnastot ja alv
	$query = "	SELECT hinnasto.tuoteno,
				hinnasto.selite,
				hinnasto.alkupvm,
				hinnasto.loppupvm,
				hinnasto.hinta,
				tuote.alv
				FROM hinnasto
				JOIN tuote on (tuote.yhtio = hinnasto.yhtio
					AND tuote.tuoteno = hinnasto.tuoteno
					AND tuote.status != 'P'
					AND tuote.tuotetyyppi NOT in ('A','B')
					AND tuote.tuoteno != ''
					AND tuote.nakyvyys != '')
				WHERE hinnasto.yhtio = '{$kukarow["yhtio"]}'
				AND (hinnasto.minkpl = 0 AND hinnasto.maxkpl = 0)
				AND hinnasto.laji != 'O'
				AND hinnasto.maa IN ('FI', '')
				AND hinnasto.valkoodi in ('EUR', '')
				$muutoslisa";
	$res = pupe_query($query);

	// Tehd��n hinnastot l�pi
	while ($row = mysql_fetch_array($res)) {

		// Jos yhti�n hinnat eiv�t sis�ll� alv:t�
		if ($yhtiorow["alv_kasittely"] != "" and $verkkokauppatyyppi != 'magento') {
			$hinta					= hintapyoristys($row["hinta"] * (1+($row["alv"]/100)));
			$hinta_veroton 			= $row["hinta"];
		}
		else {
			$hinta		 			= $row["hinta"];
			$hinta_veroton			= hintapyoristys($row["hinta"] / (1+($row["alv"]/100)));
		}

		$dnshinnasto[] = array(	'tuoteno'				=> $row["tuoteno"],
								'selite'				=> $row["selite"],
								'alkupvm'				=> $row["alkupvm"],
								'loppupvm'				=> $row["loppupvm"],
								'hinta'					=> $hinta,
								'hinta_veroton'			=> $hinta_veroton,
								);
	}

	echo date("d.m.Y @ G:i:s")." - Haetaan tuotteiden variaatiot.\n";

	// haetaan kaikki tuotteen variaatiot, jotka on menossa verkkokauppaan
	$query = "	SELECT DISTINCT tuotteen_avainsanat.selite selite
				FROM tuotteen_avainsanat
				JOIN tuote ON (tuote.yhtio = tuotteen_avainsanat.yhtio
				AND tuote.tuoteno = tuotteen_avainsanat.tuoteno
				AND tuote.status != 'P'
				AND tuote.tuotetyyppi NOT IN ('A','B')
				AND tuote.tuoteno != ''
				$nakyvyys_lisa)
				WHERE tuotteen_avainsanat.yhtio = '{$kukarow['yhtio']}'
				AND tuotteen_avainsanat.laji = 'parametri_variaatio'
				AND trim(tuotteen_avainsanat.selite) != ''";
	$resselite = pupe_query($query);

	if ($ajetaanko_kaikki == "NO") {
		$muutoslisa = " AND (tuotteen_avainsanat.muutospvm >= '{$datetime_checkpoint}'
							OR try_fi.muutospvm  >= '{$datetime_checkpoint}'
							OR ta_nimitys_se.muutospvm >= '{$datetime_checkpoint}'
							OR ta_nimitys_en.muutospvm >= '{$datetime_checkpoint}'
							OR tuote.muutospvm  >= '{$datetime_checkpoint}')";
	}
	else {
		$muutoslisa = "";
	}

	// Magentoon vain tuotteet joiden n�kyvyys != ''
	$nakyvyys_lisa = ($verkkokauppatyyppi == 'magento') ? "AND tuote.nakyvyys != ''" : "";

	// loopataan variaatio-nimitykset
	while ($rowselite = mysql_fetch_assoc($resselite)) {

		// Haetaan kaikki tuotteet, jotka kuuluu t�h�n variaatioon ja on muuttunut
		$aliselect = "	SELECT
						tuotteen_avainsanat.tuoteno,
						tuotteen_avainsanat.jarjestys,
						tuote.tunnus,
						tuote.nimitys,
						tuote.kuvaus,
						tuote.lyhytkuvaus,
						tuote.tuotemassa,
						ta_nimitys_se.selite nimi_swe,
						ta_nimitys_en.selite nimi_eng,
						tuote.myyntihinta,
						tuote.myymalahinta,
						tuote.kuluprosentti,
						tuote.eankoodi,
						tuote.alv,
						tuote.nakyvyys,
						tuote.mallitarkenne campaign_code,
						tuote.malli target,
						tuote.leimahduspiste onsale,
						try_fi.selitetark try_nimi
						FROM tuotteen_avainsanat
						JOIN tuote on (tuote.yhtio = tuotteen_avainsanat.yhtio
							AND tuote.tuoteno = tuotteen_avainsanat.tuoteno
							AND tuote.status != 'P'
							AND tuote.tuotetyyppi NOT in ('A','B')
							AND tuote.tuoteno != ''
							$nakyvyys_lisa)
						LEFT JOIN avainsana as try_fi ON (try_fi.yhtio = tuote.yhtio and try_fi.selite = tuote.try and try_fi.laji = 'try' and try_fi.kieli = 'fi')
						LEFT JOIN tuotteen_avainsanat as ta_nimitys_se on (tuote.yhtio = ta_nimitys_se.yhtio and tuote.tuoteno = ta_nimitys_se.tuoteno and ta_nimitys_se.laji = 'nimitys' and ta_nimitys_se.kieli = 'se')
						LEFT JOIN tuotteen_avainsanat as ta_nimitys_en on (tuote.yhtio = ta_nimitys_en.yhtio and tuote.tuoteno = ta_nimitys_en.tuoteno and ta_nimitys_en.laji = 'nimitys' and ta_nimitys_en.kieli = 'en')
						WHERE tuotteen_avainsanat.yhtio = '{$kukarow['yhtio']}'
						AND tuotteen_avainsanat.laji = 'parametri_variaatio'
						AND tuotteen_avainsanat.selite = '{$rowselite['selite']}'
						{$muutoslisa}
						ORDER BY tuote.tuoteno";
		$alires = pupe_query($aliselect);

		while ($alirow = mysql_fetch_assoc($alires)) {

			// Haetaan kaikki tuotteen atribuutit
			$alinselect = " SELECT tuotteen_avainsanat.selite,
							avainsana.selitetark,
							avainsana.selite option_name
							FROM tuotteen_avainsanat USE INDEX (yhtio_tuoteno)
							JOIN avainsana USE INDEX (yhtio_laji_selite) ON (avainsana.yhtio = tuotteen_avainsanat.yhtio
								AND avainsana.laji = 'PARAMETRI'
								AND avainsana.selite = SUBSTRING(tuotteen_avainsanat.laji, 11))
							WHERE tuotteen_avainsanat.yhtio='{$kukarow['yhtio']}'
							AND tuotteen_avainsanat.laji != 'parametri_variaatio'
							AND tuotteen_avainsanat.laji != 'parametri_variaatio_jako'
							AND tuotteen_avainsanat.laji like 'parametri_%'
							AND tuotteen_avainsanat.tuoteno = '{$alirow['tuoteno']}'
							AND tuotteen_avainsanat.kieli = 'fi'
							ORDER by tuotteen_avainsanat.jarjestys, tuotteen_avainsanat.laji";
			$alinres = pupe_query($alinselect);
			$properties = array();

			while ($syvinrow = mysql_fetch_assoc($alinres)) {
				$properties[] = array(	"nimi" => $syvinrow["selitetark"],
										"option_name" => $syvinrow["option_name"],
				 						"arvo" => $syvinrow["selite"]);
			}

			// Jos yhti�n hinnat eiv�t sis�ll� alv:t�
			if ($yhtiorow["alv_kasittely"] != "" and $verkkokauppatyyppi != 'magento') {
				$myyntihinta					= hintapyoristys($alirow["myyntihinta"] * (1+($alirow["alv"]/100)));
				$myyntihinta_veroton 			= $alirow["myyntihinta"];

				$myymalahinta					= hintapyoristys($alirow["myymalahinta"] * (1+($alirow["alv"]/100)));
				$myymalahinta_veroton 			= $alirow["myymalahinta"];
			}
			else {
				$myyntihinta					= $alirow["myyntihinta"];
				$myyntihinta_veroton 			= hintapyoristys($alirow["myyntihinta"] / (1+($alirow["alv"]/100)));

				$myymalahinta					= $alirow["myymalahinta"];
				$myymalahinta_veroton 			= hintapyoristys($alirow["myymalahinta"] / (1+($alirow["alv"]/100)));
			}

			$dnslajitelma[$rowselite["selite"]][] = array(	'tuoteno' 				=> $alirow["tuoteno"],
															'tunnus'				=> $alirow["tunnus"],
															'nimitys'				=> $alirow["nimitys"],
															'kuvaus'				=> $alirow["kuvaus"],
															'lyhytkuvaus'			=> $alirow["lyhytkuvaus"],
															'tuotemassa'			=> $alirow["tuotemassa"],
															'nakyvyys'				=> $alirow["nakyvyys"],
															'try_nimi'				=> $alirow["try_nimi"],
															'nimi_swe'				=> $alirow["nimi_swe"],
															'nimi_eng'				=> $alirow["nimi_eng"],
															'campaign_code'			=> $alirow["campaign_code"],
															'target'				=> $alirow["target"],
															'onsale'				=> $alirow["onsale"],
															'jarjestys'				=> $alirow["jarjestys"],
															'myyntihinta'			=> $myyntihinta,
															'myyntihinta_veroton'	=> $myyntihinta_veroton,
															'myymalahinta'			=> $myymalahinta,
															'myymalahinta_veroton'	=> $myymalahinta_veroton,
															'kuluprosentti'			=> $alirow['kuluprosentti'],
															'ean'					=> $alirow["eankoodi"],
															'parametrit'			=> $properties);
		}

	}

	$tuote_export_error_count = 0;

	echo date("d.m.Y @ G:i:s")." - Aloitetaan p�ivitys verkkokauppaan.\n";

	if (isset($verkkokauppatyyppi) and $verkkokauppatyyppi == "magento") {

		$time_start = microtime(true);

		$magento_client = new MagentoClient($magento_api_ana_url, $magento_api_ana_usr, $magento_api_ana_pas);

		if ($magento_client->getErrorCount() > 0) {
			exit;
		}

		// tax_class_id, magenton API ei anna hakea t�t� mist��n. Pit�� k�yd� katsomassa magentosta
		$magento_client->setTaxClassID($magento_tax_class_id);

		// lisaa_kategoriat
		if (count($dnstuoteryhma) > 0) {
			echo date("d.m.Y @ G:i:s")." - P�ivitet��n tuotekategoriat\n";
			$count = $magento_client->lisaa_kategoriat($dnstuoteryhma);
			echo date("d.m.Y @ G:i:s")." - P�ivitettiin $count kategoriaa\n";
		}

		// Tuotteet (Simple)
		if (count($dnstuote) > 0) {
			echo date("d.m.Y @ G:i:s")." - P�ivitet��n simple tuotteet\n";
			$count = $magento_client->lisaa_simple_tuotteet($dnstuote);
			echo date("d.m.Y @ G:i:s")." - P�ivitettiin $count tuotetta (simple)\n";
		}

		// Tuotteet (Configurable)
		if (count($dnslajitelma) > 0) {
			echo date("d.m.Y @ G:i:s")." - P�ivitet��n configurable tuotteet\n";
			$count = $magento_client->lisaa_configurable_tuotteet($dnslajitelma);
			echo date("d.m.Y @ G:i:s")." - P�ivitettiin $count tuotetta (configurable)\n";
		}

		// Saldot
		if (count($dnstock) > 0) {
			echo date("d.m.Y @ G:i:s")." - P�ivitet��n tuotteiden saldot\n";
			$count = $magento_client->paivita_saldot($dnstock);
			echo date("d.m.Y @ G:i:s")." - P�ivitettiin $count tuotteen saldot\n";
		}

		// Poistetaan tuotteet jota ei ole kaupassa
		if (count($kaikki_tuotteet) > 0) {
			echo date("d.m.Y @ G:i:s")." - Poistetaan ylim��r�iset tuotteet\n";
			// HUOM, t�h�n passataan **KAIKKI** verkkokauppatuotteet, methodi katsoo ett� kaikki n�m� on kaupassa, muut paitsi gifcard-tuotteet dellataan!
			$count = $magento_client->poista_poistetut($kaikki_tuotteet, true);
			echo date("d.m.Y @ G:i:s")." - Poistettiin $count tuotetta\n";
		}

		$tuote_export_error_count = $magento_client->getErrorCount();

		if ($tuote_export_error_count != 0) {
			echo date("d.m.Y @ G:i:s")." - P�ivityksess� tapahtui {$tuote_export_error_count} virhett�!\n";
		}

		$time_end = microtime(true);
		$time = round($time_end - $time_start);

		echo date("d.m.Y @ G:i:s")." - Tuote-export valmis! (Magento API {$time} sekuntia)\n";
	}
	elseif (isset($verkkokauppatyyppi) and $verkkokauppatyyppi == "anvia") {

		if (isset($anvia_ftphost, $anvia_ftpuser, $anvia_ftppass, $anvia_ftppath)) {
			$ftphost = $anvia_ftphost;
			$ftpuser = $anvia_ftpuser;
			$ftppass = $anvia_ftppass;
			$ftppath = $anvia_ftppath;
		}
		else {
			$ftphost = "";
			$ftpuser = "";
			$ftppass = "";
			$ftppath = "";
		}

		$tulos_ulos = "";

		if (count($dnstuote) > 0) {
			require ("{$pupe_root_polku}/rajapinnat/tuotexml.inc");
		}

		if (count($dnstock) > 0) {
			require ("{$pupe_root_polku}/rajapinnat/varastoxml.inc");
		}

		if (count($dnsryhma) > 0) {
			require ("{$pupe_root_polku}/rajapinnat/ryhmaxml.inc");
		}

		if (count($dnsasiakas) > 0) {
			require ("{$pupe_root_polku}/rajapinnat/asiakasxml.inc");
		}

		if (count($dnshinnasto) > 0) {
			require ("{$pupe_root_polku}/rajapinnat/hinnastoxml.inc");
		}

		if (count($dnslajitelma) > 0) {
			require ("{$pupe_root_polku}/rajapinnat/lajitelmaxml.inc");
		}
	}

	// Otetaan tietokantayhteys uudestaan (voi olla timeoutannu)
	unset($link);
	$link = mysql_connect($dbhost, $dbuser, $dbpass, true) or die ("Ongelma tietokantapalvelimessa $dbhost (tuote_export)");
	mysql_select_db($dbkanta, $link) or die ("Tietokantaa $dbkanta ei l�ydy palvelimelta $dbhost! (tuote_export)");
	mysql_set_charset("latin1", $link);

	// Kun kaikki onnistui, p�ivitet��n lopuksi timestamppi talteen
	$query = "	UPDATE avainsana SET
				selite = '{$datetime_checkpoint_uusi}'
				WHERE yhtio = '{$kukarow['yhtio']}'
				AND laji = 'TUOTE_EXP_CRON'";
	pupe_query($query);

	if (mysql_affected_rows() != 1) {
		echo "Timestamp p�ivitys ep�onnistui!\n";
	}
