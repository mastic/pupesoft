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

			if (empty($virhe[$i])) {
				//jos ei virheit�, asetetaan koeponnistus toimenpiteellisten huoltosyklien viimeinen_tapahtuma = laite.valm_pvm
				//t�ll�in ty�m��r�ysten generoiminen helpottuu, jos laitteelle ei ole ikin� ennen generoitu koeponnistus ty�m��r�yst�
				$query = "	SELECT tuotteen_avainsanat.selite as toimenpide_tuotteen_tyyppi
							FROM huoltosykli
							JOIN tuote
							ON ( tuote.yhtio = huoltosykli.yhtio
								AND tuote.tuoteno = huoltosykli.toimenpide )
							JOIN tuotteen_avainsanat
							ON ( tuotteen_avainsanat.yhtio = tuote.yhtio
								AND tuotteen_avainsanat.tuoteno = tuote.tuoteno
								AND tuotteen_avainsanat.laji = 'tyomaarayksen_ryhmittely')
							WHERE huoltosykli.yhtio = '{$kukarow['yhtio']}'
							AND huoltosykli.tunnus = '{$t[$i]}'";
				$result = pupe_query($query);
				$huoltosykli = mysql_fetch_assoc($result);

				if ($huoltosykli['toimenpide_tuotteen_tyyppi'] == 'koeponnistus') {
					$query = "	SELECT valm_pvm
								FROM laite
								WHERE yhtio = '{$kukarow['yhtio']}'
								AND tunnus = '{$t['laite_tunnus']}'";
					$result = pupe_query($query);
					$laite = mysql_fetch_assoc($result);

					$t['viimeinen_tapahtuma'] = $laite['valm_pvm'];
				}
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
				$virhe[$i] = t("Huoltov�li ei voi olla tyhj�").'!';
			}

			//huoltov�li ei voi olla pidempi kuin huoltoskyli_tunnus:ksen huoltov�li
			$huoltosykli = hae_huoltosykli($t['huoltosykli_tunnus']);

			if ($t[$i] > $huoltosykli['huoltovali']) {
				$virhe[$i] = t("Annettu huoltov�li ei voi olla pidmepi kuin oletushuoltosyklin huoltov�li") . " {$huoltosykli['huoltovali']}";
			}
		}

		if (mysql_field_name($result, $i) == "pakollisuus") {
			if ((int)$t[$i] != 1 and (int)$t[$i] != 0) {
				$virhe[$i] = t("Virheellinen pakollisuus sy�tetty");
			}
		}
	}
}