 <?php
 	//tarvitsemme $laskurow:n jossa on laskun tiedot
 	// ja $masrown jossa on maksuehdon tiedot

 	// jos $fakt_lahetys on tyhj� niin luodaan aineisto
 	// jos $fakt_lahetys ei ole tyhj� niin aineisto l�hetet��n

 	//softa palauttaa $sisalto-muutujan

 	$sebfact_ulos = "";

 	if ($fakt_lahetys == "") {
		//maksuehto tekstin�
		$maksuehto 		= t_tunnus_avainsanat($masrow, "teksti", "MAKSUEHTOKV");
		$kateistyyppi	= $masrow["kateinen"];

		require_once('tulosta_lasku_text.inc');

		// haetaan tilauksen kaikki rivit
		$query = "	SELECT *, concat(lpad(upper(tilausrivi.hyllyalue), 5, '0'),lpad(upper(tilausrivi.hyllynro), 5, '0'),lpad(upper(tilausrivi.hyllyvali), 5, '0'),lpad(upper(tilausrivi.hyllytaso), 5, '0')) sorttauskentta
					FROM tilausrivi
					WHERE uusiotunnus='$laskurow[tunnus]' and yhtio='$kukarow[yhtio]'
					ORDER BY otunnus, sorttauskentta, tuoteno";
		$ffaktresult = mysql_query($query) or pupe_error($query);

		//kuollaan jos yht��n rivi� ei l�ydy
		if (mysql_num_rows($ffaktresult) == 0) {
			echo t("VIRHE: Laskurivej� ei l�ytynyt");
		}
		// aloitellaan laskun teko
		$firstpage = text_alku();

		while ($row = mysql_fetch_array($ffaktresult)) {

			// Rivin toimitusaika
			if ($yhtiorow["tilausrivien_toimitettuaika"] == 'K' and $row["keratty"] == "saldoton") {
				$row["toimitettuaika"] = $row["toimaika"];
			}
			elseif ($yhtiorow["tilausrivien_toimitettuaika"] == 'X') {
				$row["toimitettuaika"] = $row["toimaika"];
			}
			else {
				$row["toimitettuaika"] = $row["toimitettuaika"];
			}

			text_rivi();
		}

		text_alvierittely();
		text_loppu();
	}

	if ($fakt_lahetys != "" and $sisalto != '') {
		//keksit��n uudelle failille joku varmasti uniikki nimi:
		$pdffilenimi  = "Autofakt-".substr(md5(uniqid(rand(),true)),0,10).".txt";

		//t�ssa on kaikki failit jotka tarvitaan
		$bound = uniqid(time()."_") ;

		$header  = "From: ".mb_encode_mimeheader($yhtiorow["nimi"], "ISO-8859-1", "Q")." <$yhtiorow[postittaja_email]>\r\n";
		$header .= "MIME-Version: 1.0\r\n" ;
		$header .= "Content-Type: multipart/mixed; boundary=\"$bound\"\r\n" ;

		$content .= "--$bound\r\n";
		$content .= "Content-Type: text/plain; name=\"$pdffilenimi\"\r\n";
		$content .= "Content-Transfer-Encoding: base64\r\n";
		$content .= "Content-Description: $pdffilenimi\r\n";
		$content .= "Content-Disposition: attachment; filename=\"$pdffilenimi\"\r\n\r\n";

		$content .= chunk_split(base64_encode($sisalto));
		$content .= "\r\n" ;

		$subjekti = $frow["sopimusnumero"]." ".$pdffilenimi;

		if($frow["email"] == '' or !mail($frow["email"], mb_encode_mimeheader($subjekti, "ISO-8859-1", "Q"), $content, $header, "-f $yhtiorow[postittaja_email]")) {
			$sebfact_ulos .= sprintf(t("VIRHE: Laskutuksen factoring-s�hk�postisiirtoa ei voitu suorittaa osoitteeseen: %s"),$frow["email"])."<br>\r\n";
		}
		else {
			$sebfact_ulos .= sprintf(t("Laskutuksen factoring-s�hk�postisiirto l�hetettiin osoitteeseen: %s"),$frow["email"])."<br><br>\r\n";
		}

		//l�hetet��n meili my�s yhti�n adminille
		mail($yhtiorow["alert_email"], mb_encode_mimeheader($subjekti, "ISO-8859-1", "Q"), $content, $header, "-f $yhtiorow[postittaja_email]");

		//echotaan rudulle jos kyseess� ei ole batch-ajo
		if ($tulos_ulos == "") {
			echo $sebfact_ulos;
		}
	}
?>