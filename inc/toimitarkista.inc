<?php

if (!function_exists("toimitarkista")) {
	function toimitarkista (&$t, $i, $result, $tunnus, &$virhe, $trow) {
		global $kukarow, $yhtiorow, $alias_set, $ulosarray, $toimtyyppi;
		static $tmp_maakoodi, $ytunnus, $hetu, $tmp_ytunnus, $tmp_valkoodi, $tmp_iban, $tmp_swift, $tmp_nimi, $tmp_ultilno_maa, $ytunnus_ind, $hetuok;
		static $tmp_edi_kuvaus, $tmp_edi_salasana, $tmp_edi_kayttaja, $tmp_edi_palvelin;
		static $tmp_edi_kuvaus_i, $tmp_edi_salasana_i, $tmp_edi_kayttaja_i, $tmp_edi_palvelin_i;

		$fieldname = mysql_field_name($result, $i);

		if (($fieldname == "nimi") or
			($fieldname == "maa") or
			($fieldname == "ytunnus") or
			($fieldname == "oletus_valkoodi")) {

			if (trim($t[$i]) == '') {
				$virhe[$i] = t("Tieto puuttuu");
			}

			if ($fieldname == "maa") {
				$t[$i] = strtoupper($t[$i]);
				$tmp_maakoodi = $t[$i];
			}
		}

		if ($fieldname == "oletus_hyvak1") {
			if ($alias_set == "KAYTTAJA") {
				$t[$i] = $tmp_nimi;
			}

			if (trim($t[$i]) == '' and ($yhtiorow['jalkilaskenta_kuluperuste'] != 'VS' or $alias_set == "KAYTTAJA")) {
				$virhe[$i] = t("Tieto puuttuu");
			}
		}

		if ($fieldname == "nimi") {
			$tmp_nimi = $t[$i];
		}

		if ($fieldname == "ultilno_maa") {
			$tmp_ultilno_maa = $t[$i];
		}

		if ($fieldname == "tilino" and $t[$i] != "") {
			$query = "	SELECT yhtio
						FROM tili
						WHERE yhtio = '$kukarow[yhtio]'
						and tilino = '$t[$i]'";
			$sresult = pupe_query($query);

			if (mysql_num_rows($sresult) != 1) {
				$virhe[$i] = t("Tili� ei l�ydy");
			}
		}

		if ($fieldname == "tilinumero") {
			$t[$i] = trim($t[$i]);

			if ($tmp_maakoodi == strtoupper($yhtiorow['maa'])) {
				if ((int) trim($t[$i]) != 0) {
					$pankkitili = $t[$i];

					if ($tmp_maakoodi == 'FI') {
						require 'pankkitilinoikeellisuus.php';

						if ($pankkitili != '') {
							$t[$i]     = $pankkitili;
							$vastaus   = luoiban($t[$i]);
							$tmp_iban  = $vastaus['iban'];
							$tmp_swift = $vastaus['swift'];
						}
					}
				}
			}
		}

		if ($fieldname == "ultilno") {

			if ($t[$i] == '') $t[$i] = $tmp_iban;

			// Vaaditaan isot kirjaimet
			$t[$i] = strtoupper(trim($t[$i]));

			if ($tmp_ultilno_maa != "") $ibanmaa = $tmp_ultilno_maa;
			else $ibanmaa = $tmp_maakoodi;

			if ($t[$i] == '' and $yhtiorow['jalkilaskenta_kuluperuste'] != 'VS') {
				$virhe[$i] = t("Tieto puuttuu");
			}

			// Luodaan swift, jos sit� ei olla sy�tetty ja tili on suomalainen
			if (substr($t[$i], 0, 2) == "FI" and $tmp_swift == "") {
				include ("inc/pankkitiedot.inc");
				$vastaus = pankkitiedot($t[$i],'');
				$tmp_swift = $vastaus['swift'];
			}

			if ($t[$i] != '') {
				// Jos SEPA-maa, tarkistetaan IBAN
				if (tarkista_sepa($ibanmaa) and tarkista_iban($t[$i]) != $t[$i]) {
					$virhe[$i] = t("Virheellinen IBAN!")." $t[$i]";
				}
				elseif (tarkista_bban($t[$i]) === FALSE) {
					$virhe[$i] = t("Virheellinen BBAN!")." $t[$i] ".t("Tilinumerossa saa olla vain kirjaimia A-Z ja/tai numeroita 0-9");
				}
			}
		}

		if ($fieldname == "swift") {

			if ($t[$i] == '') $t[$i] = $tmp_swift;

			$t[$i] = strtoupper(trim($t[$i]));

			if ($t[$i] == '' and $yhtiorow['jalkilaskenta_kuluperuste'] != 'VS') {
				$virhe[$i] = t("Tieto puuttuu");
			}

			if ($tmp_ultilno_maa != "") $ibanmaa = $tmp_ultilno_maa;
			else $ibanmaa = $tmp_maakoodi;

			if ($t[$i] != '') {
				// Jos SEPA-maa, tarkistetaan BIC
				if (tarkista_sepa($ibanmaa) and tarkista_bic($t[$i]) === FALSE) {
					$virhe[$i] = t("Virheellinen BIC!")." $t[$i]";
				}
			}
		}

		if ($fieldname == "ytunnus" and trim($t[$i]) != '') {
			$ytunnus = trim($t[$i]);
			$ytunnus_ind = $i;

			if ($tmp_maakoodi == 'FI' and ($yhtiorow["ytunnus_tarkistukset"] == "" or $yhtiorow["ytunnus_tarkistukset"] == "T")) {


				if (strlen($ytunnus) == 11) {
					$hetu = $ytunnus;

					// katotaan oliko kyseess� hetu
					require ("tarkistahetu.inc");

					// ei ollu oikee hetukaan
					if ($hetuok == 1) {
						$virhe[$i] = t("Virheellinen henkil�tunnus!");
					}
				}
				else {
					require ("tarkistaytunnus.inc");

					if ($ok == 0) {
						$virhe[$i] = t("Virheellinen ytunnus!")." $t[$i] -> $ytunnus. ".t("Voit tarkistaa ytunnuksen")." <a target='uusiytj' href='http://www.ytj.fi/yrit_sel2.asp?kielikoodi=1'>".t("t��lt�")."</a>.";
					}
					else {
						$t[$i] = $ytunnus;
					}
				}
			}
			else { // ulkomaalainen saa olla mit� se haluaa
				$t[$i] = $ytunnus;
			}

			$tmp_ytunnus = $t[$i];

			// Jos ytunnus vaihtuu niin korjataan my�s tuotteiden toimittajat
			if (trim($t[$i]) != "" and $trow[$i] != $t[$i] and $errori != 1 and (int) $tunnus > 0) {
				$query = "	UPDATE tuotteen_toimittajat
							SET toimittaja = '$t[$i]'
							WHERE tuotteen_toimittajat.yhtio		= '$kukarow[yhtio]'
							and tuotteen_toimittajat.liitostunnus 	= $tunnus";
				$vres = pupe_query($query);
			}
		}

		if ($fieldname == "ohjeitapankille") {

			if ($tmp_maakoodi=='FI' and strlen($t[$i]) != '') {
				$virhe[$i] = t("K�yt�ss� vain ulkomaisille toimittajille");
			}
			else {
				$t[$i]=trim($t[$i]);
				if (strlen($t[$i]) > 350) {
					$virhe[$i] = t("Maksimipituus on 350 merkki�");
				}
			}
		}

		if ($fieldname == "ovttunnus") {
			if ($tmp_maakoodi == 'FI') {

				if (trim($t[$i]) == '' and !isset($hetuok)) {
					// t�ytet��n ovttunnus jos sit� ei ole, eik� ytunnus oo henkil�tunnus
					$t[$i] = "0037".sprintf("%08d", preg_replace("/[^0-9]/", "", $tmp_ytunnus));
				}

				$query = "	SELECT ytunnus, tunnus
							FROM toimi
							WHERE yhtio	  = '$kukarow[yhtio]'
							and ytunnus   = '$tmp_ytunnus'
							and ovttunnus = '$t[$i]'
							and tyyppi 	 != 'P'
							and tunnus   != '$tunnus'";
				$sresult = pupe_query($query);

				if (mysql_num_rows($sresult) > 0) {
					$virhe[$i] = t("Samalla ytunnuksella %s on useita toimittajia! Lis�� tarkenne ovt-tunnukseen.", "", $tmp_ytunnus);
				}
			}
		}

		if ($fieldname == "email" and trim($t[$i]) != '') {
			// Tarkistetaan s�hk�postiosoite
			$email_virhe = tarkista_email($t[$i]);

			if ($email_virhe != "") {
				$virhe[$i] = t("S�hk�postin domain ei l�ydy")."! ($email_virhe)";
			}
		}

		if ($fieldname == "oletus_valkoodi") {
			$tmp_valkoodi = $t[$i];
		}

		if ($fieldname == "tyyppi") {
			$toimtyyppi = $t[$i]; // otetaan t�� talteen

			if ($toimtyyppi == "K") {
				$query = "	SELECT ytunnus
							FROM toimi
							WHERE yhtio = '$kukarow[yhtio]'
							and ytunnus = '$ytunnus'
							and tyyppi  = 'K'
							and tunnus != '$tunnus'";
				$vres = pupe_query($query);

				if (mysql_num_rows($vres) > 0)	{
					$virhe[$i] = $virhe[$ytunnus_ind] = t("Henkil� on jo olemassa!");
				}
			}
		}

		if ($fieldname == "tyyppi_tieto") {

			if ($toimtyyppi == "S") {
				$query = "SELECT * from yhtio where konserni='$yhtiorow[konserni]' and yhtio='$t[$i]' and yhtio!='$kukarow[yhtio]'";
				$tyychkre = pupe_query($query);

				if (mysql_num_rows($tyychkre) == 0) {
					$virhe[$i] = "V��r� yhti�tunnus: $t[$i]!";
				}
			}

			if ($toimtyyppi == "O" and trim($t[$i]) == "") {
				$virhe[$i] = "Tieto puuttuu!";
			}

			if ($toimtyyppi == "L" and $tunnus > 0 and $errori != 1) {
				// injisoidaan tuotteen_toimittajat kaikille tuotteille
				$query = "	INSERT into tuotteen_toimittajat (yhtio, tuoteno, toim_tuoteno, toimittaja, liitostunnus, valuutta, alkuperamaa, tuotekerroin, laatija, luontiaika)
							SELECT yhtio, tuoteno, tuoteno, '$tmp_ytunnus', '$tunnus', '$tmp_valkoodi', '$tmp_maakoodi', 1, '$kukarow[kuka]', now()
							FROM tuote
							WHERE tuote.yhtio='$kukarow[yhtio]' and tuote.ei_saldoa='' and tuote.status != 'P'
							ON DUPLICATE KEY UPDATE
							tuotteen_toimittajat.liitostunnus 	= $tunnus,
							tuotteen_toimittajat.toimittaja		= '$tmp_ytunnus',
							tuotteen_toimittajat.valuutta		= '$tmp_valkoodi',
							tuotteen_toimittajat.alkuperamaa	= '$tmp_maakoodi'";
				$vres = pupe_query($query);
			}

			if ($toimtyyppi == "PP" and $tunnus > 0 and $errori != 1) {
				// poistetaan tuotteen_toimittajat kaikille tuotteille
				$query = "	SELECT lasku.yhtio, group_concat(DISTINCT tilausrivi.otunnus) tilaukset
							from lasku
							join tilausrivi on tilausrivi.yhtio=lasku.yhtio and tilausrivi.otunnus=lasku.tunnus and tilausrivi.tyyppi='O' and tilausrivi.varattu != 0
							where lasku.yhtio		= '$kukarow[yhtio]'
							and lasku.liitostunnus	= '$tunnus'
							and lasku.tila = 'O'
							group by 1";
				$tyychkre1 = pupe_query($query);

				$query = "	SELECT lasku.yhtio, group_concat(DISTINCT lasku.laskunro) tilaukset
							from lasku
							where lasku.yhtio		= '$kukarow[yhtio]'
							and lasku.liitostunnus	= '$tunnus'
							and lasku.tila 	= 'K'
							and lasku.alatila != 'X'
							and lasku.vanhatunnus = 0
							group by 1";
				$tyychkre2 = pupe_query($query);

				if (mysql_num_rows($tyychkre1) == 0 and mysql_num_rows($tyychkre2) == 0) {

					$query = "	DELETE FROM tuotteen_toimittajat
								WHERE tuotteen_toimittajat.yhtio		= '$kukarow[yhtio]'
								and tuotteen_toimittajat.liitostunnus 	= $tunnus";
					$vres = pupe_query($query);
				}
				else {
					$tyychkro1 = mysql_fetch_array($tyychkre1);
					$tyychkro2 = mysql_fetch_array($tyychkre2);

					$virhe[$i] = "Tuotteen toimittajia ei voitu poistaa!<br>Tilaukset: ($tyychkro1[tilaukset]) ovat auki.<br>Keikat: ($tyychkro2[tilaukset]) ovat auki.";
				}
			}
		}

		if ($fieldname == "edi_palvelin") {
			$tmp_edi_palvelin = trim($t[$i]);
			$tmp_edi_palvelin_i = $i;
		}

		if ($fieldname == "edi_kayttaja") {
			$tmp_edi_kayttaja = trim($t[$i]);
			$tmp_edi_kayttaja_i = $i;
		}

		if ($fieldname == "edi_salasana") {
			$tmp_edi_salasana = trim($t[$i]);
			$tmp_edi_salasana_i = $i;
		}

		if ($fieldname == "edi_kuvaus") {
			$tmp_edi_kuvaus = trim($t[$i]);
			$tmp_edi_kuvaus_i = $i;
		}

		if ($fieldname == "edi_polku") {
			$tmp_edi_polku = trim($t[$i]);

			if ($tmp_edi_kuvaus == 'editilaus_out_wabco.inc' or $tmp_edi_kuvaus == 'editilaus_out_excel.inc' or $tmp_edi_kuvaus == 'editilaus_out_xml.inc' or $tmp_edi_kuvaus == 'sendorder_out_futur') {
				// N�iss� tyypeiss� ei tartte olle mit��n kentti� sy�tetty
			}
			elseif ($tmp_edi_kuvaus != "" and $tmp_edi_palvelin == "localhost" and ($tmp_edi_polku == "" or !is_writable($tmp_edi_polku))) {
				$tmp_virhe = t("Paikallinen siirto ei onnistu, koska hakemistoon ei ole kirjoitusoikeutta")."!";
				$virhe[$i] = $tmp_virhe;
				$virhe[$tmp_edi_palvelin_i] = $tmp_virhe;
			}
			elseif ($tmp_edi_kuvaus != "" and $tmp_edi_palvelin != "localhost" and ($tmp_edi_palvelin == "" or $tmp_edi_kayttaja == "" or $tmp_edi_salasana == "" or $tmp_edi_polku == "")) {
				$tmp_virhe = t("Edi-kuvaus valittu, mutta l�hett�miseen tarvittavia kentti� ei sy�tetty")."!";
				$virhe[$i] = $tmp_virhe;
				$virhe[$tmp_edi_palvelin_i] = $tmp_virhe;
				$virhe[$tmp_edi_kayttaja_i] = $tmp_virhe;
				$virhe[$tmp_edi_salasana_i] = $tmp_virhe;
				$virhe[$tmp_edi_kuvaus_i] = $tmp_virhe;
			}
		}

	}
}
