<?php

require_once('inc/laite_huolto_functions.inc');

$ulos = "";
$jatko = 1; // oletetaan normaali k�sittely
$tyyppi = 1; // oletetaan rivin n�kyvyys

if (mysql_field_name($result, $i) == "yhtio") {
	$tyyppi = 0;
}

if (mysql_field_type($result, $i) == "blob") {
	$ulos = "<td><textarea cols='50' rows='5' name='$nimi'>$trow[$i]</textarea></td>";
	$jatko = 0;
}

if (mysql_field_name($result, $i) == "huoltosykli_tunnus") {

	//jos lukitse avaimeen, niin ollaan n�ytt�m�ss� huoltosyklit_laitteita iframessa laite n�kym�ss�
	//t�ll�in haetaan laitteen tiedot sek� paikka ja dropdownissa n�ytet��n vain kyseisen laitteen huoltosyklit
	if (!empty($lukitse_avaimeen)) {
		$laite = hae_laite($lukitse_avaimeen);
		$laite['paikka'] = hae_paikka($laite['paikka']);
	}

	$huoltosyklit = hae_laitteelle_mahdolliset_huoltosyklit($laite['laite_tyyppi'], $laite['laite_koko'], $laite['paikka']['olosuhde']);

	$ulos = "<td>";
	$ulos .= "<select class='{$otsikko}' name='{$nimi}'>";
	$sel = "";
	foreach ($huoltosyklit as $huoltosykli) {
		if ($trow[$i] == $huoltosykli['tunnus']) {
			$sel = "SELECTED";
		}
		$ulos .= "<option value='{$huoltosykli['tunnus']}' {$sel}>".t($huoltosykli['dropdown_text'])."</option>";
		$sel = "";
	}
	$ulos .= "</select>";
	$ulos .= "</td>";

	$jatko = 0;
}

if (mysql_field_name($result, $i) == "laite_tunnus") {
	// Jos t�m� on liitetty tunnuksella:
	if ($trow[$i] > 0) {
		$ulos = "<input type='hidden' name='$nimi' value='$trow[$i]'>";
	}
	// Jos t�� on uusi itemi
	elseif ((int)$tunnus == 0) {
		$ulos = "<input type='hidden' name='$nimi' value='$lukitse_avaimeen'>";
	}

	$tyyppi = 4;
	$jatko = 0;
}

//huoltov�li float aluksi annetaan p�iviss�, my�hemmin voi olla mittarilukema, k�ytt�kerta
if (mysql_field_name($result, $i) == "huoltovali") {
	
}

//pakollisuus kyll� / ei
if (mysql_field_name($result, $i) == "pakollisuus") {
	$ulos = "<td>";
	$ulos .= "<select class='{$otsikko}' name='{$nimi}'>";
	$ulos .= "<option value=1>".t("Kyll�")."</option>";
	$ulos .= "<option value=0>".t("Ei")."</option>";
	$ulos .= "</select>";
	$ulos .= "</td>";

	$jatko = 0;
}

if (mysql_field_name($result, $i) == "viimeinen_tapahtuma") {
	$tyyppi = 0;
}