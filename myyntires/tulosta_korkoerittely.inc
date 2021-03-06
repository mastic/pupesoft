<?php

	function paperi_alku () {
		global $pdf, $asiakastiedot, $yhteyshenkilo, $yhtiorow, $kukarow, $kala, $sivu, $rectparam, $norm, $pieni, $bold, $kaatosumma, $kieli, $vmehto, $trow;

		$firstpage = $pdf->new_page("a4");

		//Haetaan yhteyshenkilon tiedot
		$apuqu = "	SELECT *
					from kuka
					where yhtio='$kukarow[yhtio]' and tunnus='$yhteyshenkilo'";
		$yres = pupe_query($apuqu);
		$yrow = mysql_fetch_assoc($yres);

		//Otsikko
		//$pdf->draw_rectangle(830, 20,  800, 580, $firstpage, $rectparam);
		$pdf->draw_text(30, 815,  $yhtiorow["nimi"], $firstpage);
		$pdf->draw_text(280, 815, t("Korkoerittely", $kieli), $firstpage, $bold);
		$pdf->draw_text(430, 815, t("Sivu", $kieli)." ".$sivu, $firstpage, $norm);

		//Vasen sarake
		//$pdf->draw_rectangle(737, 20,  674, 300, $firstpage, $rectparam);
		$pdf->draw_text(50, 729, t("Laskutusosoite", $kieli), $firstpage, $pieni);
		$pdf->draw_text(50, 717, $asiakastiedot["nimi"], $firstpage, $norm);
		$pdf->draw_text(50, 707, $asiakastiedot["nimitark"], $firstpage, $norm);
		$pdf->draw_text(50, 697, $asiakastiedot["osoite"], $firstpage, $norm);
		$pdf->draw_text(50, 687, $asiakastiedot["postino"]." ".$asiakastiedot["postitp"], $firstpage, $norm);
		$pdf->draw_text(50, 677, $asiakastiedot["maa"], $firstpage, $norm);

		//Oikea sarake
		$pdf->draw_rectangle(800, 300, 779, 580, $firstpage, $rectparam);
		$pdf->draw_rectangle(800, 420, 779, 580, $firstpage, $rectparam);
		$pdf->draw_text(310, 792, t("P�iv�m��r�", $kieli), $firstpage, $pieni);

		if (basename($_SERVER["SCRIPT_FILENAME"]) != 'muutosite.php')
			$pdf->draw_text(310, 782, tv1dateconv(date('Y-m-d')), $firstpage, $norm);
		else
			$pdf->draw_text(310, 782, tv1dateconv($trow['tapvm']), $firstpage, $norm);

		$pdf->draw_text(430, 792, t("Asiaa hoitaa", $kieli), $firstpage, $pieni);
		$pdf->draw_text(430, 782, $yrow["nimi"], $firstpage, $norm);

		$pdf->draw_rectangle(779, 300, 758, 580, $firstpage, $rectparam);
		$pdf->draw_rectangle(779, 420, 758, 580, $firstpage, $rectparam);
		$pdf->draw_text(310, 771, t("Er�p�iv�", $kieli), $firstpage, $pieni);


		if (basename($_SERVER["SCRIPT_FILENAME"]) != 'muutosite.php') {
			// Etsit��n maksuehto
			$query = "	SELECT *
						FROM maksuehto
						WHERE tunnus='$vmehto' and yhtio='$kukarow[yhtio]'";
			$presult = pupe_query($query);
			$xrow = mysql_fetch_assoc($presult);

			if ($xrow['abs_pvm'] == '0000-00-00') {

				$seurday   = sprintf('%02d', date("j", mktime(0, 0, 0, date("n"), date("j")+$xrow["rel_pvm"],  date("Y"))));
				$seurmonth = sprintf('%02d', date("n", mktime(0, 0, 0, date("n"), date("j")+$xrow["rel_pvm"],  date("Y"))));
				$seuryear  = sprintf('%02d', date("Y", mktime(0, 0, 0, date("n"), date("j")+$xrow["rel_pvm"],  date("Y"))));

				$erapvm = $seuryear."-".$seurmonth."-".$seurday;
			}
			else {
				$erapvm = "'$xrow[abs_pvm]'";
			}
		}
		else {
			$erapvm = $trow['erpcm'];
		}

		$pdf->draw_text(310, 761, tv1dateconv($erapvm), $firstpage, $norm);

		$pdf->draw_text(430, 771, t("Puhelin", $kieli),	$firstpage, $pieni);
		$pdf->draw_text(430, 761, $yrow["puhno"], 		$firstpage, $norm);

		$pdf->draw_rectangle(758, 300, 737, 580, $firstpage, $rectparam);
		$pdf->draw_rectangle(758, 420, 737, 580, $firstpage, $rectparam);
		$pdf->draw_text(310, 750, t("Ytunnus/Asiakasnumero", $kieli), 	$firstpage, $pieni);
		$pdf->draw_text(310, 740, $asiakastiedot["ytunnus"], 			$firstpage, $norm);

		$pdf->draw_text(430, 750, t("S�hk�posti", $kieli), 	$firstpage, $pieni);
		$pdf->draw_text(430, 740, $yrow["eposti"], 			$firstpage, $norm);

		$kala = 620;

		//Laskurivien otsikkotiedot
		//otsikko
		$pdf->draw_text(30, $kala+30, t("Laskut", $kieli), $firstpage, $bold);

		//eka rivi
		$pdf->draw_text(30,  $kala, t("Laskunumero", $kieli),		$firstpage, $pieni);
		$pdf->draw_text(100, $kala, t("P�iv�m��r�", $kieli),		$firstpage, $pieni);
		$pdf->draw_text(160, $kala, t("Er�p�iv�", $kieli),			$firstpage, $pieni);
		$pdf->draw_text(220, $kala, t("Maksettu", $kieli),			$firstpage, $pieni);
		$pdf->draw_text(280, $kala, t("My�h�ss� pv.", $kieli),		$firstpage, $pieni);
		$pdf->draw_text(340, $kala, t("Viiv�stykorko-%", $kieli),	$firstpage, $pieni);
		$pdf->draw_text(390, $kala, t("Laskun summa", $kieli),		$firstpage, $pieni);
		$pdf->draw_text(485, $kala, t("Korko", $kieli),				$firstpage, $pieni);
		$pdf->draw_text(545, $kala, t("Yhteens�", $kieli),	$firstpage, $pieni);

		$kala -= 15;

		return($firstpage);
	}

	function paperi_rivi ($firstpage, $korkosumma, $summa) {
		global $yhtiorow, $firstpage, $pdf, $row, $kala, $sivu, $lask, $rectparam, $norm, $pieni, $lask, $kieli;

		if ($lask >= 37) {
			$sivu++;
			paperi_loppu($firstpage,'');
			$firstpage = paperi_alku();
			$kala = 605;
			$lask = 1;
		}

		$pdf->draw_text(30,  $kala, $row["laskunro"],								$firstpage, $norm);
		$pdf->draw_text(100, $kala, tv1dateconv($row["tapvm"]), 					$firstpage, $norm);
		$pdf->draw_text(160, $kala, tv1dateconv($row["erpcm"]), 					$firstpage, $norm);
		$pdf->draw_text(220, $kala, tv1dateconv($row["mapvm"]), 					$firstpage, $norm);

		$oikpos = $pdf->strlen($row["ika"], $norm);
		$pdf->draw_text(300-$oikpos, $kala, $row["ika"],							$firstpage, $norm);
		
		$pdf->draw_text(340, $kala, $row["viikorkopros"],							$firstpage, $norm);

		$oikpos = $pdf->strlen($row["summa"], $norm);
		$pdf->draw_text(455-$oikpos, $kala, $row["summa"], 							$firstpage, $norm);

		$oikpos = $pdf->strlen($row["korkosumma"], $norm);
		$pdf->draw_text(505-$oikpos, $kala, $row["korkosumma"], 					$firstpage, $norm);

		$oikpos = $pdf->strlen(sprintf('%.2f', $row["summa"] + $row["korkosumma"]), $norm);
		$pdf->draw_text(565-$oikpos, $kala, sprintf('%.2f', $row["summa"] + $row["korkosumma"]),	$firstpage, $norm);

		$kala = $kala - 13;

		$lask++;

		$korkosumma+=$row["korkosumma"];
		$summa+=$row["summa"];

		$palautus = array(
			"korkosumma" => $korkosumma,
			"summa" 	 => $summa,
		);

		return($palautus);
	}

	function paperi_loppu ($firstpage, $korkosumma, $summa, $valkoodi, $masrow = "") {
		global $pdf, $laskurow, $yhtiorow, $kukarow, $sivu, $rectparam, $norm, $pieni, $kaatosumma, $kieli, $toimipaikkarow;

		//yhteens�rivi
		if ($korkosumma == '') {
			$pdf->draw_text(420, 75, t('Jatkuu...',$kieli), $firstpage, $norm);
		}
		else {
			$pdf->draw_text(400, 105, t('Laskut yhteens�', $kieli), $firstpage, $norm);
			$pdf->draw_text(400, 90,  t('Korko yhteens�', $kieli), $firstpage, $norm);
			$pdf->draw_text(400, 75,  t('Yhteens�', $kieli), $firstpage, $norm);

			$summa = sprintf('%.2f', $summa);
			$korkosumma	= sprintf('%.2f', $korkosumma);
			$kokkukaikkisumma = sprintf('%.2f', $summa+$korkosumma);

			$oikpos = $pdf->strlen($summa, $norm);
			$pdf->draw_text(535-$oikpos, 105, $summa, $firstpage, $norm);

			$oikpos = $pdf->strlen($korkosumma, $norm);
			$pdf->draw_text(535-$oikpos, 90, $korkosumma, $firstpage, $norm);

			$oikpos = $pdf->strlen($kokkukaikkisumma, $norm);
			$pdf->draw_text(535-$oikpos, 75, $kokkukaikkisumma, $firstpage, $norm);

			$pdf->draw_text(550, 105, $valkoodi, $firstpage, $norm);
			$pdf->draw_text(550, 90, $valkoodi, $firstpage, $norm);
			$pdf->draw_text(550, 75, $valkoodi, $firstpage, $norm);

			//Kokonaissummalaatikko + valuuttalaatikko
			$pdf->draw_rectangle(120, 394, 70, 580,	$firstpage, $rectparam);
			$pdf->draw_rectangle(120, 540, 70, 580,	$firstpage, $rectparam);
		}

		//Alimmat kolme laatikkoa, yhti�tietoja
		$pdf->draw_rectangle(70, 20, 20, 580,	$firstpage, $rectparam);
		$pdf->draw_rectangle(70, 207, 20, 580,	$firstpage, $rectparam);
		$pdf->draw_rectangle(70, 394, 20, 580,	$firstpage, $rectparam);

		$pdf->draw_text(30, 55, $toimipaikkarow["nimi"],		$firstpage, $pieni);
		$pdf->draw_text(30, 45, $toimipaikkarow["osoite"],	$firstpage, $pieni);
		$pdf->draw_text(30, 35, $toimipaikkarow["postino"]."  ".$toimipaikkarow["postitp"],	$firstpage, $pieni);
		$pdf->draw_text(30, 25, $toimipaikkarow["maa"],		$firstpage, $pieni);

		$pdf->draw_text(217, 55, t("Puhelin", $kieli).":",		$firstpage, $pieni);
		$pdf->draw_text(247, 55, $toimipaikkarow["puhelin"],			$firstpage, $pieni);
		$pdf->draw_text(217, 45, t("Fax", $kieli).":",			$firstpage, $pieni);
		$pdf->draw_text(247, 45, $toimipaikkarow["fax"],				$firstpage, $pieni);
		$pdf->draw_text(217, 35, t("Email", $kieli).":",		$firstpage, $pieni);
		$pdf->draw_text(247, 35, $toimipaikkarow["email"],			$firstpage, $pieni);

		$pdf->draw_text(404, 55, t("Y-tunnus", $kieli).":",		$firstpage, $pieni);
		$pdf->draw_text(444, 55, $toimipaikkarow["vat_numero"],			$firstpage, $pieni);
		$pdf->draw_text(404, 45, t("Kotipaikka", $kieli).":",	$firstpage, $pieni);
		$pdf->draw_text(444, 45, $toimipaikkarow["kotipaikka"],		$firstpage, $pieni);
		$pdf->draw_text(404, 35, t("Enn.per.rek", $kieli),		$firstpage, $pieni);
		$pdf->draw_text(404, 25, t("Alv.rek", $kieli),			$firstpage, $pieni);

	}

	require('pdflib/phppdflib.class.php');

	//echo "<font class='message'>Korkoerittely tulostuu...</font>";
	flush();

	//PDF parametrit
	$pdf = new pdffile;
	$pdf->set_default('margin-top', 	0);
	$pdf->set_default('margin-bottom', 	0);
	$pdf->set_default('margin-left', 	0);
	$pdf->set_default('margin-right', 	0);
	$rectparam["width"] = 0.3;

	$norm["height"] = 10;
	$norm["font"] = "Times-Roman";

	$pieni["height"] = 8;
	$pieni["font"] = "Times-Roman";

	$bold["height"] = 14;
	$bold["font"] = "Helvetica-Bold";

	// defaultteja
	$lask = 1;
	$sivu = 1;

	// aloitellaan laskun teko
	$xquery='';
	for ($i=0; $i < count($lasku_tunnus); $i++) {
		if ($i != 0) {
			$xquery=$xquery . ",";
		}

		$xquery=$xquery . "$lasku_tunnus[$i]";
	}

	if (basename($_SERVER["SCRIPT_FILENAME"]) == 'muutosite.php') {
		$query = "	SELECT tunnus
					FROM kuka
					WHERE yhtio = '$kukarow[yhtio]'
					and kuka = '$trow[laatija]'";
		$result = pupe_query($query);
		$yrow = mysql_fetch_assoc($result);
		$yhteyshenkilo = $yrow['tunnus'];

		$query = "	SELECT group_concat(tunnus) tunnukset
					FROM asiakas
					WHERE yhtio = '$kukarow[yhtio]'
					and ytunnus = '$trow[ytunnus]'";
		$result = pupe_query($query);
		$asiakasrow2 = mysql_fetch_assoc($result);

		$query = "	SELECT l.tunnus, l.liitostunnus, t.summa*-1 summa, l.erpcm, l.yhtio_toimipaikka,
					l.laskunro, t.tapvm mapvm, l.tapvm tapvm, l.tunnus, l.viikorkopros, to_days(t.tapvm) - to_days(l.erpcm) as ika,
					round(l.viikorkopros * t.summa * -1 * (to_days(t.tapvm)-to_days(l.erpcm)) / 36500,2) as korkosumma, l.maksuehto,
					l.valkoodi
					FROM lasku l
					join tiliointi t use index (tositerivit_index) on (t.yhtio = l.yhtio and t.tilino in ('$yhtiorow[myyntisaamiset]', '$yhtiorow[factoringsaamiset]') and t.tapvm > l.erpcm and t.tapvm <= '$trow[tapvm]' and l.tunnus = t.ltunnus)
					LEFT JOIN maksuehto me on (me.yhtio=l.yhtio and me.tunnus=l.maksuehto)
					WHERE l.liitostunnus in ($asiakasrow2[tunnukset])
					and l.yhtio = '$kukarow[yhtio]'
					and l.tila = 'U'
					and l.mapvm > '0000-00-00'
					and l.olmapvm = '$trow[tapvm]'
					ORDER BY l.laskunro, t.tapvm";
	}
	else {
		$query = "	SELECT l.tunnus, l.liitostunnus, t.summa*-1 summa, l.erpcm, l.yhtio_toimipaikka,
					l.laskunro, t.tapvm mapvm, l.tapvm tapvm, l.tunnus, l.viikorkopros, to_days(t.tapvm) - to_days(l.erpcm) as ika,
					round(l.viikorkopros * t.summa * -1 * (to_days(t.tapvm)-to_days(l.erpcm)) / 36500,2) as korkosumma, l.maksuehto,
					l.valkoodi
					FROM lasku l use index (PRIMARY)
					join tiliointi t use index (tositerivit_index) on (t.yhtio = l.yhtio and t.tilino in ('$yhtiorow[myyntisaamiset]', '$yhtiorow[factoringsaamiset]') and t.tapvm > l.erpcm and l.tunnus = t.ltunnus)
					LEFT JOIN maksuehto me on (me.yhtio=l.yhtio and me.tunnus=l.maksuehto)
					WHERE l.tunnus in ($xquery)
					and l.yhtio = '$kukarow[yhtio]'
					and l.tila = 'U'
					ORDER BY l.laskunro, t.tapvm";
	}

	$result = pupe_query($query);
	//otetaan asiakastiedot ekalta laskulta
	$asiakastiedot = mysql_fetch_assoc($result);

	if ($asiakastiedot["yhtio_toimipaikka"] != '' and $asiakastiedot["yhtio_toimipaikka"] != 0) {
		$toimipaikkaquery = "	SELECT *
								FROM yhtion_toimipaikat
								WHERE yhtio = '$kukarow[yhtio]'
								AND tunnus  = '$asiakastiedot[yhtio_toimipaikka]'";
		$toimipaikkares = pupe_query($toimipaikkaquery);
		$toimipaikkarow = mysql_fetch_assoc($toimipaikkares);
	}
	else {
		$toimipaikkarow["nimi"] 		= $yhtiorow["nimi"];
		$toimipaikkarow["osoite"] 		= $yhtiorow["osoite"];
		$toimipaikkarow["postino"] 		= $yhtiorow["postino"];
		$toimipaikkarow["postitp"] 		= $yhtiorow["postitp"];
		$toimipaikkarow["maa"] 			= $yhtiorow["maa"];
		$toimipaikkarow["puhelin"] 		= $yhtiorow["puhelin"];
		$toimipaikkarow["fax"] 			= $yhtiorow["fax"];
		$toimipaikkarow["email"] 		= $yhtiorow["email"];
		$toimipaikkarow["vat_numero"] 	= $yhtiorow["ytunnus"];
		$toimipaikkarow["kotipaikka"] 	= $yhtiorow["kotipaikka"];
	}

	$query = "	SELECT *
				FROM asiakas
				WHERE yhtio='$kukarow[yhtio]'
				AND tunnus = '$asiakastiedot[liitostunnus]'";
	$asiakasresult = pupe_query($query);
	$asiakastiedot = mysql_fetch_assoc($asiakasresult);

	$query = "	SELECT *
				FROM maksuehto
				WHERE yhtio='$kukarow[yhtio]'
				AND tunnus = '$asiakastiedot[maksuehto]'";
	$mehtores = pupe_query($query);
	$mehtorow = mysql_fetch_assoc($mehtores);

	//Otetaan t�ss� asiakkaan kieli talteen
	$kieli = $asiakastiedot["kieli"];

	//Liitostunnusta tarvitaan tee_korkolasku.inc failissa
	$liitostunnus  = $asiakastiedot["tunnus"];

	//ja kelataan akuun
	mysql_data_seek($result,0);

	$firstpage = paperi_alku();

	//karhu_begin_work();
	$summa = 0;
	$korkosumma = 0;

	while ($row = mysql_fetch_assoc($result)) {
		$palautus = paperi_rivi($firstpage,$korkosumma,$summa);

		$korkosumma = $palautus['korkosumma'];
		$summa = $palautus['summa'];

		//p�ivitet��n korkosumma
		if (basename($_SERVER["SCRIPT_FILENAME"]) != 'muutosite.php') {
			$query = "	UPDATE lasku
						SET viikorkoeur = '$row[korkosumma]'
						WHERE tunnus = '$row[tunnus]'
						and yhtio = '$kukarow[yhtio]'";
			$eurresult = pupe_query($query);
		}
	}

	$loppusumma = sprintf('%.2f', $summa);
	$loppukorkosumma = sprintf('%.2f', $korkosumma);
	$valkoodi = $asiakastiedot['valkoodi'];

	paperi_loppu($firstpage, $loppukorkosumma, $loppusumma, $valkoodi, $mehtorow);

	if (basename($_SERVER["SCRIPT_FILENAME"]) != 'muutosite.php') {
		//keksit��n uudelle failille joku varmasti uniikki nimi:
		list($usec, $sec) = explode(' ', microtime());
		mt_srand((float) $sec + ((float) $usec * 100000));
		$pdffilenimi = "/tmp/korkoerittely-".md5(uniqid(mt_rand(), true)).".pdf";

		//kirjoitetaan pdf faili levylle..
		$fh = fopen($pdffilenimi, "w");
		if (fwrite($fh, $pdf->generate()) === FALSE) die("PDF kirjoitus ep�onnistui $pdffilenimi");
		fclose($fh);

		// itse print komento...
		$query = "	SELECT komento
					from kirjoittimet
					where yhtio = '$kukarow[yhtio]'
					and tunnus = '$kirjoitin'";
		$kires = pupe_query($query);

		if (mysql_num_rows($kires) == 1) {
			$kirow = mysql_fetch_assoc($kires);

			if ($kirow["komento"] == 'email') {
				$liite = $pdffilenimi;
				$kutsu = "korkoerittely-$asiakastiedot[ytunnus]";
				$ctype = "pdf";
				require("../inc/sahkoposti.inc");
			}
			else {
				$line  = exec("$kirow[komento] $pdffilenimi");
			}

			//system("rm -f $pdffilenimi");
			unset($pdffilenimi);
		}
	}
	else {
		echo $pdf->generate();
	}
	unset($pdf);
	unset($firstpage);
?>