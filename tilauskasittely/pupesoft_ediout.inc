<?php

/*
// pupesoft s�hk�inen tilaus
// rivitunniste 9 merkki�, kaksoispiste (:), tieto
// kaikki kuvauksen rivitunnisteet ovat pakollisia
// kaikki muunlaiset rivit ohitetaan

ICHGSTART:						// Tiedosto alkaa
ICHG_RCPT:003707495288			// Tilauksen vastaanottajan ovttunnus

OMSGSTART:						// Tilaus alkaa

OHDRSTART:						// Tilauksen otsikko alkaa
OBGMINUMB:2740969				// Tilaajan tilausnumero (ei pakollinen tieto)
OBGMIACKR:FS					// Tilausvahvistuksen muoto F=fax ja/tai S=s�hk�posti (ei pakollinen tieto)
OBGMITYPE:						// Pupesoft tilaustyyppi (ei pakollinen)
ONADBY_PC:003706270618			// Tilaajan OVT-tunnus (ei pakollinen tieto jos asiakasnumero annettu)
ONADBY_RF:3145   				// Tilaajan asiakasnumero toimittajalla (ei pakollinen tieto jos OVT-tunnus annettu)
ONADDP_NA:Asse Asiakas			// Poikkeava toimitusnimi (ei pakollinen tieto)
ONADDP_SA:Testikatu 7 			// Poikkeava toimitusosoite (ei pakollinen tieto)
ONADDP_PO:Vantaa 				// Poikkeava toimituspostitoimipaikka (ei pakollinen tieto)
ONADDP_CI:01510  				// Poikkeava toimituspostinro (ei pakollinen tieto)
OFTXDELTX:Kaukokiito    	 	// Toivottu toimitustapa (ei pakollinen tieto)
OFTXGENTX:Kommentti				// Tilauksen kommentti, n�kyy l�hetteell�. (ei pakollinen tieto)
ORFFABONU:Viite					// Tilauksen viite, n�kyy l�hetteell� ja laskulla. (ei pakollinen tieto)
OHDR__END:						// Tilauksen otsikko loppuu

// Rivien segmentit on seuraavanlaisia

OLINSTART:144532				// Rivit alkaa / tilaajan ostotilauksen rivinumero (ei pakollinen tieto)
OLINSA_NU:314012				// Tilattava tuotenumero
OLIN21_QT:12					// Tilattava m��r�
OLIN__END:144532				// Tilausrivi loppuu
OLINSA_PI:						// varasto josta rivi on myyty

// Kun kaikki rivit on k�sitely

OMSG__END:						// Tilaus loppuu

ICHG__END:						// Tiedosto loppuu

*/

// tarvitaan laskurow (ostotilauksen otsikko), yhtiorow ja kukarow
$pupesoft_ediout_ulos = "";

if (!function_exists("dateconv")) {
	function dateconv ($date) {
		//k��nt�� mysqln vvvv-kk-mm muodon muotoon vvvvkkmm
		return substr($date,0,4).substr($date,5,2).substr($date,8,2);
	}
}

$plkulsa = "";
if ($kukarow["extranet"] == "") {
	$plkulsa = "../";
}

$nimi = $plkulsa."dataout/pupesoft-order-$kukarow[yhtio]-".date("Ymd")."-".md5(uniqid(rand(),true)).".txt";

if (!$toot = fopen($nimi, "w")) die("Filen $nimi luonti ep�onnistui!");

$query = "	SELECT tilausrivi.tunnus, toim_tuoteno, ostohinta, eankoodi, tilausrivi.tuoteno, round(varattu*if(tuotteen_toimittajat.tuotekerroin=0 or tuotteen_toimittajat.tuotekerroin is null,1,tuotteen_toimittajat.tuotekerroin),4) varattu, toimaika,
			tilausrivi.hyllyalue, kommentti, hinta
			FROM tilausrivi
			LEFT JOIN tuote USING (yhtio, tuoteno)
			LEFT JOIN tuotteen_toimittajat ON tuotteen_toimittajat.yhtio=tilausrivi.yhtio and tuotteen_toimittajat.tuoteno=tilausrivi.tuoteno and tuotteen_toimittajat.liitostunnus='$laskurow[liitostunnus]'
			WHERE otunnus = '$laskurow[tunnus]' 
			and varattu > 0 
			and tilausrivi.yhtio='$kukarow[yhtio]'
			ORDER BY toim_tuoteno";
$result = mysql_query($query) or pupe_error($query);

if (mysql_num_rows($result) == 0) {
	$pupesoft_ediout_ulos .= "<font class='error'>L�hetett�vi� tilausrivej� ei l�ydy</font>";
}
else {

	// katotaan ollaanko valittu tilaajaksi joku muu toimipiste
	if ($laskurow["vanhatunnus"] != 0) {
		$query  = "SELECT * from yhtion_toimipaikat where yhtio='$kukarow[yhtio]' and tunnus='$laskurow[yhtio_toimipaikka]'";
		$ediotr = mysql_query($query) or pupe_error($query);

		if (mysql_num_rows($ediotr) == 1) {
			$edioro = mysql_fetch_array($ediotr);
		}
		else {
			$edioro = array(); // tyhj��
		}
	}

	$ulos  = "ICHGSTART:\n";										// Tiedosto alkaa
	$ulos .= "ICHG_RCPT:$laskurow[ovttunnus]\n";					// Tilauksen vastaanottajan ovttunnus

	$ulos .= "OMSGSTART:\n";										// Tilaus alkaa

	$ulos .= "OHDRSTART:\n";										// Tilauksen otsikko alkaa.
	$ulos .= "OBGMINUMB:$laskurow[tunnus]\n";						// Meid�n tilausnumero
	$ulos .= "OBGMIACKR:S\n";										// Tilausvahvistus F fax, S s�hk�posti
	$ulos .= "OBGMITYPE:$laskurow[ketjutus]\n";						// Pupesoft tilaustyyppi
	$ulos .= "ONADBY_PC:$yhtiorow[ovttunnus]$edioro[ovtlisa]\n";	// Meid�n ovttunnus + toimipaikan ovtlisa jos sellanen on
	$ulos .= "ONADBY_RF:\n";										// Meid�n asiakasnumero toimittajalla
	$ulos .= "ONADDP_NA:$laskurow[toim_nimi]\n";					// Poikkeava toimitusnimi
	$ulos .= "ONADDP_NB:$laskurow[toim_nimitark]\n";				// Poikkeava toimitusnimitark
	$ulos .= "ONADDP_SA:$laskurow[toim_osoite]\n";					// Poikkeava toimitusosoite
	$ulos .= "ONADDP_CI:$laskurow[toim_postitp]\n";					// Poikkeava toimituspostitoimipaikka
	$ulos .= "ONADDP_PO:$laskurow[toim_postino]\n";					// Poikkeava toimituspostinro
	$ulos .= "OFTXDELTX:$laskurow[toimitustapa]\n";					// Toivottu toimitustapa
	$ulos .= "OFTXGENTX:$laskurow[comments]\n";						// Tilauksen otsikon kommentti
	$ulos .= "ORFFABONU:$laskurow[viesti]\n";						// Tilauksen otsikon viite
	$ulos .= "OHDR__END:\n";										// Tilauksen otsikko loppuu

	while ($tilausrivirow = mysql_fetch_array ($result)) {

		// jos ei olla sy�tetty toimittajan tuotenumeroa niin k�ytet��n meid�n omaa.. hmm?
		if ($tilausrivirow["toim_tuoteno"] == "") $tilausrivirow["toim_tuoteno"] = $tilausrivirow["tuoteno"];

		$ulos .= "OLINSTART:$tilausrivirow[tunnus]\n";				// Meid�n ostotilauksen rivinumero
		$ulos .= "OLINSA_NU:$tilausrivirow[toim_tuoteno]\n";		// Tilattava tuotenumero
		$ulos .= "OLIN21_QT:$tilausrivirow[varattu]\n";				// Tilattava m��r� ja yksikk� (yksikk� ei pakollinen)

		// kolmanteen segmenttiin varaston tunnus jos se tiedet��n
		if ($tilausrivirow['hyllyalue'] != '') {
			$ulos .= "OLINSA_PI:@@$tilausrivirow[hyllyalue]\n";
		}

		// viidenteen segmenttiin menee sarjanumeron tunnus jos se tiedet��n, lis�t��n my�s t�ss� keisiss� riville mukaan tuotteen hinta koska vastaanottaja luottaa meihin
		if ($tilausrivirow['kommentti'] != '') {
//			$ulos .= "OLIN8__IF:@@@@@$tilausrivirow[kommentti]\n";
//			$ulos .= "OLINCONIV:" . sprintf("%015.2f", (float) $tilausrivirow['hinta'])."\n";
		}

		$ulos .= "OLIN__END:$tilausrivirow[tunnus]\n";				// Tilausrivi loppuu
	}

	$ulos .= "OMSG__END:\n"; 										// Tilaus loppuu
	$ulos .= "ICHG__END:\n"; 										// Tiedosto loppuu

	fputs($toot,$ulos);
	fclose($toot);

	$palautus = 0;

	if ($toimirow['edi_palvelin'] == '' or $toimirow['edi_kayttaja'] == '' or $toimirow['edi_salasana'] == '' or $toimirow['edi_polku'] == '') {
		$pupesoft_ediout_ulos .= "<font class='error'>".t("Ostotilauksen l�hetykseen tarvittavia tietoja puuttuu toimittajalta")."! (host, user, pass, path)</font><br>";
		$palautus = 1;
	}
	else {
		$conn_id = ftp_connect($toimirow['edi_palvelin']);

		// jos connectio ok, kokeillaan loginata
		if ($conn_id) {
			$login_result = ftp_login($conn_id, $toimirow['edi_kayttaja'], $toimirow['edi_salasana']);
		}

		// jos login ok kokeillaan uploadata
		if ($login_result) {
			$upload = ftp_put($conn_id, $toimirow['edi_polku']."/".basename($nimi), realpath($nimi), FTP_ASCII);
		}

		if ($conn_id) {
			ftp_close($conn_id);
		}

		// jos mik�tahansa feilas niin palautetaan yks
		if ($conn_id === FALSE or $login_result === FALSE or $upload === FALSE) {
			$palautus = 1;
		}

		if ($palautus == 0) $pupesoft_ediout_ulos .= "<font class='message'>Ostotilaus l�hetettiin onnistuneesti toimittajalle!</font><br><br>";
		if ($palautus == 1) $pupesoft_ediout_ulos .= "<font class='error'>Tilauksen siirto toimittajalle ep�onnistui!</font><br><br>";
	}
}

$nimi  = "";
$ulos  = "";

if ($silent == "") {
	echo $pupesoft_ediout_ulos;
}

?>