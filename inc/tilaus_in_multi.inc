<?php

require_once("tilauskasittely/luo_myyntitilausotsikko.inc");

if (!$fd = fopen($filename, "r")) die("Filen '$filename' avaus ep�onnistui!");

$edytunnus = "luuppiinmeno_ekalla";

/* FILEFORMAATTI:
	ytunnus
	toim_ovttunnus
	tuoteno
	kpl
	alv
	toimaika
	hinta
	ale
	netto
	var
	kommentti
*/

while ($rivi = fgets($fd)) {

	$poista	  	= array("\"", "'", "\\");
	$rivi	  	= str_replace($poista,"",$rivi);
	$rivi	  	= explode("\t", trim($rivi));

	$ytunnus		= trim($rivi[0]);
	$toim_ovttunnus	= trim($rivi[1]);
	$tuoteno		= trim($rivi[2]);

	$tarkytunnus = $ytunnus.$toim_ovttunnus;

	if ($ytunnus != "" and $tuoteno != "") {

 		// asiakas vaihtuu, uusi otsikko
 		if ($tarkytunnus != $edytunnus) {

			if (isset($tilaus_valmiiksi) and $edytunnus != "luuppiinmeno_ekalla") {
				require("tilaus-valmis.inc");
			}

 			$query = "	UPDATE kuka
						SET kesken=0
						WHERE session = '$session'";
			$result = pupe_query($query);

			$kukarow['kesken'] 	= 0;
			$tilausnumero 		= 0;

			if ($toim_ovttunnus != '') {
				$toim_lis = " and toim_ovttunnus='$toim_ovttunnus' ";
			}
			else {
				$toim_lis = "";
			}

			$query = "	SELECT tunnus
 						FROM asiakas
 						WHERE yhtio = '$kukarow[yhtio]'
						and ytunnus = '$ytunnus'
						$toim_lis
						and laji not in ('P','R')";
 			$result = pupe_query($query);

 			if (mysql_num_rows($result) == 0) {
 				echo t("VIRHE: Tiedoilla ei l�ydy yht��n asiakasta").": ytunnus = '$ytunnus' $toim_lis!<br>";

				// Skipataan t�m� rivi.....
				continue;
			}
			elseif (mysql_num_rows($result) > 1) {
 				echo t("VIRHE: Tiedoilla l�ytyy useita asiakkaita").": ytunnus = '$ytunnus' $toim_lis!<br>";

				// Skipataan t�m� rivi.....
				continue;
 			}

 			$arow = mysql_fetch_array($result);

			$id = luo_myyntitilausotsikko("RIVISYOTTO", $arow["tunnus"]);

 			echo "Tehtiin tilaus $id asiakkaalle $ytunnus<br>";

 			// haetaan tilauksen tiedot
 			$query    = "SELECT * from lasku where tunnus='$id' and yhtio='$kukarow[yhtio]'";
 			$result   = mysql_query($query) or die($query);
 			$laskurow = mysql_fetch_array($result);

 		}

 		$query = "	SELECT *
 					FROM tuote
 					WHERE tuoteno = '$tuoteno'
					and yhtio = '$kukarow[yhtio]'";
 		$rarresult = pupe_query($query);

 		if (mysql_num_rows($rarresult) == 1) {
 			$trow = mysql_fetch_array($rarresult);

			$ytunnus         = $laskurow["ytunnus"];
			$kpl             = (float) str_replace(",", ".", $rivi[3]);
			$tuoteno         = $trow["tuoteno"];
			$toimaika 	     = $rivi[5] != "" ? $rivi[5] : $laskurow["toimaika"];
			$kerayspvm	     = $laskurow["kerayspvm"];
			$hinta 		     = str_replace(",", ".", $rivi[6]);
			$netto 		     = strtoupper(trim($rivi[8]));
			$alv		     = str_replace(",", ".", $rivi[4]);
			$var			 = strtoupper(trim($rivi[9]));
			$varasto 	     = "";
			$rivitunnus		 = "";
			$korvaavakielto	 = "";
			$jtkielto 		 = $laskurow['jtkielto'];
			$varataan_saldoa = "";
			$myy_sarjatunnus = "";
			$kommentti		 = trim($rivi[10]);
			$paikka			 = "";

			for ($alepostfix = 1; $alepostfix <= $yhtiorow['myynnin_alekentat']; $alepostfix++) {
				if ($alepostfix == 1) ${'ale'.$alepostfix} = str_replace(",", ".", $rivi[7]);
				else ${'ale'.$alepostfix} = '';
			}

			if ($hinta == 0) {
 				$hinta = "";
 			}

 			//lis�t��n rivi
 			require ("lisaarivi.inc");
 		}

 		$edytunnus = $tarkytunnus;

	}
	
}

if (isset($tilaus_valmiiksi)) {
	require("tilaus-valmis.inc");
}

fclose ($fd);
