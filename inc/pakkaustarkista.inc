<?php

	if (!function_exists("pakkaustarkista")) {
		function pakkaustarkista (&$t, $i, $result, $tunnus, &$virhe, $trow) {
			global $yhtiorow;

			if (mysql_field_name($result, $i) == "pakkaus") {
				if (trim($t[$i]) == '') {
					$virhe[$i] .= t("Pakkaus ei saa olla tyhj�")."!";
				}
				else {
					$t[$i] = trim(mysql_real_escape_string($t[$i]));
				}
			}

			if (mysql_field_name($result, $i) == "pakkauskuvaus") {
				if (trim($t[$i]) == '') {
					$virhe[$i] .= t("Pakkauskuvaus ei saa olla tyhj�")."!";
				}
				else {
					$t[$i] = trim(mysql_real_escape_string($t[$i]));
				}
			}

			if (mysql_field_name($result, $i) == "korkeus" or mysql_field_name($result, $i) == "leveys" or mysql_field_name($result, $i) == "syvyys" or mysql_field_name($result, $i) == "paino") {
				$t[$i] = trim(str_replace(",", ".", $t[$i]));

				if ($yhtiorow['varastopaikkojen_maarittely'] == 'M' and ($t[$i] == '' or !is_numeric($t[$i]) or $t[$i] == 0)) {
					$virhe[$i] .= t("Mitta- ja painotiedot eiv�t saa olla tyhji�")."!";
				}
				elseif ($t[$i] != '') {
					$t[$i] = mysql_real_escape_string($t[$i]);
				}
			}

			if (mysql_field_name($result, $i) == 'kayttoprosentti') {
				$t[$i] = (int) $t[$i];

				if ($t[$i] == 0) $t[$i] = 100;
			}
		}
	}
