<?php

if (!function_exists("huoltosyklit_laitteet")) {

	function huoltosyklit_laitteettarkista(&$t, $i, $result, $tunnus, &$virhe, $trow) {
		global $kukarow, $yhtiorow, $alias_set, $alasveto, $asiakasid;

		if (mysql_field_name($result, $i) == "huoltosykli_tunnus") {
			$query = "	SELECT *
						FROM huoltosykli
						WHERE yhtio = '{$kukarow['yhtio']}'
						AND tunnus = {$t[$i]}";
			$huoltosykli_result = pupe_query($query);

			if (mysql_num_rows($huoltosykli_result) == 0) {
				$virhe[$i] .= t("Huoltosykli� ei ole olemassa");
			}
		}

		if (mysql_field_name($result, $i) == "laite_tunnus") {
			$query = "	SELECT *
						FROM laite
						WHERE yhtio = '{$kukarow['yhtio']}'
						AND tunnus = {$t[$i]}";
			$laite_result = pupe_query($query);

			if (mysql_num_rows($laite_result) == 0) {
				$virhe[$i] .= t("Laitetta ei ole olemassa");
			}
		}

		if (mysql_field_name($result, $i) == "huoltovali") {
			if (!is_numeric($t[$i])) {
				$virhe[$i] = t("Huoltov�li annettava p�ivin�");
				//$virhe[$i] = t("Huoltov�li annettava p�ivin�, mittarilukemana tai k�ytt�kertana");
			}

			if (empty($t[$i])) {
				$virhe[$i] = t("Huoltov�li ei voi olla tyhj�!");
			}
		}

		if (mysql_field_name($result, $i) == "pakollisuus") {
			if ((int)$t[$i] != 1 and (int)$t[$i] != 0) {
				$virhe[$i] = t("Virheellinen pakollisuus sy�tetty");
			}
		}
	}
}