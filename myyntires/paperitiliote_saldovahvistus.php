<?php

require('pdflib/phppdflib.class.php');

function hae_saldovahvistus_pdf($saldovahvistus) {
	global $kukarow, $yhtiorow, $pdf, $kala, $sivu, $norm, $pieni, $kieli, $bold, $lask, $rectparam, $asiakastiedot;

	//PDF parametrit
	$pdf = new pdffile;
	$pdf->set_default('margin-top', 0);
	$pdf->set_default('margin-bottom', 0);
	$pdf->set_default('margin-left', 0);
	$pdf->set_default('margin-right', 0);
	$rectparam = array();
	$rectparam["width"] = 0.3;
	$lask = 1;
	$sivu = 1;

	$norm["height"] = 10;
	$norm["font"] = "Times-Roman";
	$pieni["height"] = 8;
	$pieni["font"] = "Times-Roman";
	$bold["height"] = 10;
	$bold["font"] = "Times-Bold";


	//Otetaan t�ss� asiakkaan kieli talteen
	$kieli = $saldovahvistus['asiakas']['kieli'];

	$firstpage = alku($saldovahvistus);

	$x[0] = 30;
	$x[1] = 565;
	$y[0] = $y[1] = $kala + 25;
	$pdf->draw_line($x, $y, $firstpage, $rectparam);

	foreach ($saldovahvistus['laskut'] as $row) {
		$firstpage = rivi(1, $firstpage, $row, $saldovahvistus);
	}

	$y[0] = $y[1] = $kala + 5;
	$pdf->draw_line($x, $y, $firstpage, $rectparam);

	loppu($firstpage, $saldovahvistus);

	//keksit��n uudelle failille joku varmasti uniikki nimi:
	list($usec, $sec) = explode(' ', microtime());
	mt_srand((float)$sec + ((float)$usec * 100000));
	$pdffilenimi = "/tmp/saldovahvistus-".md5(uniqid(mt_rand(), true)).".pdf";

	//kirjoitetaan pdf faili levylle..
	$fh = fopen($pdffilenimi, "w");
	if (fwrite($fh, $pdf->generate()) === FALSE) {
		die("PDF kirjoitus ep�onnistui $pdffilenimi");
	}
	fclose($fh);

	return $pdffilenimi;
}

function alku($saldovahvistus) {
	global $pdf, $asiakastiedot, $yhtiorow, $kukarow, $kala, $sivu, $norm, $pieni, $kieli, $bold;

	$firstpage = $pdf->new_page("a4");

	tulosta_logo_pdf($pdf, $firstpage, "");

	//Otsikko
	$pdf->draw_text(280, 815, t("Saldovahvistus", $kieli), $firstpage);
	$pdf->draw_text(430, 815, t("Sivu", $kieli)." ".$sivu, $firstpage, $norm);

	if ($asiakastiedot["laskutus_nimi"] != "") {
		$pdf->draw_text(30, 750, $asiakastiedot["laskutus_nimi"], $firstpage, $bold);
		$pdf->draw_text(30, 740, $asiakastiedot["laskutus_nimitark"], $firstpage, $bold);
		$pdf->draw_text(30, 730, $asiakastiedot["laskutus_osoite"], $firstpage, $bold);
		$pdf->draw_text(30, 720, $asiakastiedot["laskutus_postino"]." ".$asiakastiedot["laskutus_postitp"], $firstpage, $bold);
		$pdf->draw_text(30, 710, $asiakastiedot["laskutus_maa"], $firstpage, $bold);
	}
	else {
		$pdf->draw_text(30, 750, $asiakastiedot["nimi"], $firstpage, $bold);
		$pdf->draw_text(30, 740, $asiakastiedot["nimitark"], $firstpage, $bold);
		$pdf->draw_text(30, 730, $asiakastiedot["osoite"], $firstpage, $bold);
		$pdf->draw_text(30, 720, $asiakastiedot["postino"]." ".$asiakastiedot["postitp"], $firstpage, $bold);
		$pdf->draw_text(30, 710, $asiakastiedot["maa"], $firstpage, $bold);
	}

	$pdf->draw_text(380, 780, t("P�iv�m��r�", $kieli).': '.date('d.m.Y'), $firstpage, $norm);

	//Oikea sarake
	$pdf->draw_text(380, 760, t("Laatija", $kieli).':', $firstpage, $norm);
	$pdf->draw_text(440, 760, $kukarow["nimi"], $firstpage, $norm);

	$pdf->draw_text(380, 750, t("Puhelin", $kieli).':', $firstpage, $norm);
	$pdf->draw_text(440, 750, $kukarow["puhno"], $firstpage, $norm);

	$pdf->draw_text(380, 740, t("Fax", $kieli).':', $firstpage, $norm);
	$pdf->draw_text(440, 740, $saldovahvistus['asiakas']['fax'], $firstpage, $norm);

	$pdf->draw_text(380, 730, t("S�hk�posti", $kieli).':', $firstpage, $norm);
	$pdf->draw_text(440, 730, $kukarow["eposti"], $firstpage, $norm);

	$pdf->draw_text(30, 650, t('Ilmoitamme ett� avoin saldo ', $kieli)." ".date('d.m.Y', strtotime($saldovahvistus['laskun_avoin_paiva']))." ".t('on', $kieli), $firstpage, $norm);
	$pdf->draw_text(300, 650, $saldovahvistus['avoin_saldo_summa'], $firstpage, $norm);
	$pdf->draw_text(350, 650, $saldovahvistus['valkoodi'], $firstpage, $norm);

	$pdf->draw_one_paragraph(640, 30, 540, ($pdf->currentPage['width'] - 30), $saldovahvistus['saldovahvistus_viesti']['selitetark'], $firstpage, $norm);

	//Rivit alkaa t�s� kohtaa
	$object_keys = array_keys($pdf->objects);
	$kala = ($pdf->objects[max($object_keys)]['bottom']) - 20;

	//eka rivi
	$pdf->draw_text(30, $kala, t("Laskunro", $kieli), $firstpage, $pieni);
	$pdf->draw_text(100, $kala, t("P�iv�m��r�", $kieli), $firstpage, $pieni);
	$pdf->draw_text(180, $kala, t("Er�p�iv�", $kieli), $firstpage, $pieni);

	$oikpos = $pdf->strlen(t("Avoinsumma", $kieli), $pieni);
	$pdf->draw_text(480 - $oikpos, $kala, t("Avoinsumma", $kieli), $firstpage, $pieni);

	$kala -= 15;

	return($firstpage);
}

function rivi($tyyppi, $firstpage, $row, $saldovahvistus) {
	global $pdf, $kala, $sivu, $lask, $norm, $pieni, $yhtiorow;

	if ($lask == 19) {
		$sivu++;
		loppu($firstpage, array());
		$firstpage = alku($saldovahvistus);
		$kala = 510;
		$lask = 1;
	}

	$pdf->draw_text(30, $kala, $row["laskunro"], $firstpage, $norm);
	$pdf->draw_text(100, $kala, tv1dateconv($row["tapvm"]), $firstpage, $norm);
	$pdf->draw_text(180, $kala, tv1dateconv($row["erpcm"]), $firstpage, $norm);

	$oikpos = $pdf->strlen($row["avoin_saldo"], $norm);
	$pdf->draw_text(480 - $oikpos, $kala, $row["avoin_saldo"], $firstpage, $norm);

	$kala = $kala - 13;
	$lask++;
	return($firstpage);
}

function loppu($firstpage, $saldovahvistus) {
	global $pdf, $yhtiorow, $kukarow, $sivu, $rectparam, $norm, $pieni, $kieli, $lask, $kala, $bold, $asiakastiedot;

	if ($lask > 35) {
		$sivu++;
		loppu($firstpage, array());
		$firstpage = alku();
		$kala = 605;
		$lask = 1;
	}

	if (!empty($saldovahvistus)) {
		$kala -= 20;
		$pdf->draw_text(250, $kala, t('Avoin saldo yhteens�', $kieli).'('.t('alv mukana', $kieli).')', $firstpage, $bold);
		$pdf->draw_text(452.5, $kala, $saldovahvistus['avoin_saldo_summa'], $firstpage, $bold);
		$pdf->draw_text(500, $kala, $saldovahvistus['valkoodi'], $firstpage, $bold);
	}

	$kala = 270;

	//Pankkiyhteystiedot
	$pdf->draw_rectangle($kala, 30, ($kala - 45), 565, $firstpage, $rectparam);

	$pdf->draw_text(40, $kala - 8, t("Pankkiyhteys", $kieli), $firstpage, $pieni);
	$pdf->draw_text(40, $kala - 18, $yhtiorow["pankkinimi1"]." ".$yhtiorow["pankkitili1"], $firstpage, $norm);
	$pdf->draw_text(40, $kala - 28, $yhtiorow["pankkinimi2"]." ".$yhtiorow["pankkitili2"], $firstpage, $norm);
	$pdf->draw_text(40, $kala - 38, $yhtiorow["pankkinimi3"]." ".$yhtiorow["pankkitili3"], $firstpage, $norm);

	$pdf->draw_text(230, $kala - 18, (empty($yhtiorow["pankkiiban1"])) ? '' : 'IBAN: '.$yhtiorow["pankkiiban1"], $firstpage, $norm);
	$pdf->draw_text(230, $kala - 28, (empty($yhtiorow["pankkiiban2"])) ? '' : 'IBAN: '.$yhtiorow["pankkiiban1"], $firstpage, $norm);
	$pdf->draw_text(230, $kala - 38, (empty($yhtiorow["pankkiiban3"])) ? '' : 'IBAN: '.$yhtiorow["pankkiiban1"], $firstpage, $norm);

	$pdf->draw_text(430, $kala - 18, (empty($yhtiorow["pankkiswift1"])) ? '' : 'SWIFT: '.$yhtiorow["pankkiswift1"], $firstpage, $norm);
	$pdf->draw_text(430, $kala - 28, (empty($yhtiorow["pankkiswift2"])) ? '' : 'SWIFT: '.$yhtiorow["pankkiswift2"], $firstpage, $norm);
	$pdf->draw_text(430, $kala - 38, (empty($yhtiorow["pankkiswift3"])) ? '' : 'SWIFT: '.$yhtiorow["pankkiswift3"], $firstpage, $norm);

	$kala = 225;

	//Alimmat kolme laatikkoa, yhti�tietoja
	$pdf->draw_rectangle($kala, 30, ($kala - 50), 565, $firstpage, $rectparam);
	$pdf->draw_rectangle($kala, 207, ($kala - 50), 565, $firstpage, $rectparam);
	$pdf->draw_rectangle($kala, 394, ($kala - 50), 565, $firstpage, $rectparam);
	$pdf->draw_text(40, $kala - 13, $yhtiorow["nimi"], $firstpage, $pieni);
	$pdf->draw_text(40, $kala - 23, $yhtiorow["osoite"], $firstpage, $pieni);
	$pdf->draw_text(40, $kala - 33, $yhtiorow["postino"]."  ".$yhtiorow["postitp"], $firstpage, $pieni);
	$pdf->draw_text(40, $kala - 43, $yhtiorow["maa"], $firstpage, $pieni);

	$pdf->draw_text(217, $kala - 13, t("Puhelin", $kieli).":", $firstpage, $pieni);
	$pdf->draw_text(247, $kala - 13, $yhtiorow["puhelin"], $firstpage, $pieni);
	$pdf->draw_text(217, $kala - 23, t("Fax", $kieli).":", $firstpage, $pieni);
	$pdf->draw_text(247, $kala - 23, $yhtiorow["fax"], $firstpage, $pieni);
	$pdf->draw_text(217, $kala - 33, t("Email", $kieli).":", $firstpage, $pieni);
	$pdf->draw_text(247, $kala - 33, $yhtiorow["email"], $firstpage, $pieni);

	$pdf->draw_text(404, $kala - 13, t("Y-tunnus", $kieli).":", $firstpage, $pieni);
	$pdf->draw_text(444, $kala - 13, $yhtiorow["ytunnus"], $firstpage, $pieni);
	$pdf->draw_text(404, $kala - 23, t("Kotipaikka", $kieli).":", $firstpage, $pieni);
	$pdf->draw_text(444, $kala - 23, $yhtiorow["kotipaikka"], $firstpage, $pieni);
	$pdf->draw_text(404, $kala - 33, t("Enn.per.rek", $kieli), $firstpage, $pieni);
	$pdf->draw_text(404, $kala - 43, t("Alv.rek", $kieli), $firstpage, $pieni);


	$kala = 162;
	//Katkoviiva
	$y = array();
	$y[0] = $y[1] = $kala;
	$how_many_lines = 70;
	$page_width = $pdf->currentPage['width'];
	$margin_width = 30;
	$line_and_empty_space_width = ($page_width - ($margin_width * 2)) / $how_many_lines;
	for ($i = $margin_width; $i <= ($page_width - $margin_width); $i = $i + $line_and_empty_space_width) {
		$x[0] = $i;
		$line_width = $line_and_empty_space_width * 0.75;
		$x[1] = $i + $line_width;
		$pdf->draw_line($x, $y, $firstpage, $rectparam);
	}

	$pdf->draw_text(30, $kala - 20, t('Saldovahvistus', $kieli), $firstpage, $bold);

	$pdf->draw_text(30, $kala - 40, t('Todistamme ett�', $kieli)." {$asiakastiedot['nimi']} ".t('velka', $kieli).'/'.t('ennakkomaksu', $kieli)." {$yhtiorow['nimi']} ".date('d.m.Y', strtotime($saldovahvistus['laskun_avoin_paiva'])).' '.t('on', $kieli), $firstpage, $bold);

	$x[0] = 30;
	$x[1] = 230;
	$y[0] = $y[1] = $kala - 70;
	$pdf->draw_line($x, $y, $firstpage, $rectparam);
	$pdf->draw_text(235, $kala - 70, 'EUROT', $firstpage, $bold);

	$pdf->draw_text(30, $kala - 90, t('Nimi', $kieli).':', $firstpage, $bold);
	$y[0] = $y[1] = $kala - 90;
	$x[0] = 100;
	$x[1] = 320;
	$pdf->draw_line($x, $y, $firstpage, $rectparam);

	$pdf->draw_text(30, $kala - 110, t('Allekirjoitus', $kieli).':', $firstpage, $bold);
	$y[0] = $y[1] = $kala - 110;
	$x[0] = 100;
	$x[1] = 320;
	$pdf->draw_line($x, $y, $firstpage, $rectparam);

	$pdf->draw_text(30, $kala - 130, t('Puhelin', $kieli).':', $firstpage, $bold);
	$y[0] = $y[1] = $kala - 130;
	$x[0] = 100;
	$x[1] = 320;
	$pdf->draw_line($x, $y, $firstpage, $rectparam);
}