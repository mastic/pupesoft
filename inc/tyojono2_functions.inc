<?php

function hae_tyojono2_js() {
	global $palvelin2;
	
	ob_start();
	?>
	<script src='<?php echo $palvelin2 ?>js/tyomaarays/tyojono2.js'></script>
	<script>
		$(document).ready(function() {
			if ($('#tyojono_wrapper').length > 0) {
				$('#tyojono_wrapper').tyojonoPlugin();
				var tyojono_plugin = $('#tyojono_wrapper').data('tyojonoPlugin');

				tyojono_plugin.bind_kohde_tr_click();
				tyojono_plugin.bind_select_kaikki_checkbox();
				tyojono_plugin.bind_laite_checkbox();
				tyojono_plugin.bind_aineisto_submit_button_click();
			}
		});
	</script>
	<?php

	$js = ob_get_clean();

	return $js;
}

function hae_tyojono2_css() {
	ob_start();
	?>
	<style>
		.laite_table_tr_hidden {
			display: none;
		}

		.laite_table_tr {
			background-color: #DADDDE;
		}

		.laite_tr {
			background-color: #DADDDE;
		}
	</style>
	<?php

	$js = ob_get_clean();

	return $js;
}

function hae_tyomaaraykset($request) {
	global $kukarow, $yhtiorow;

	$lasku_where = "";
	if (!empty($request['lasku_tunnukset'])) {
		$lasku_where = "AND lasku.tunnus IN ('".implode("','", $request['lasku_tunnukset'])."')";
	}

	//t�m� k�yt�ss� asiakaan_laite_hallinta.php
	if (!empty($request['liitostunnus'])) {
		$lasku_where .= "	AND lasku.liitostunnus = '{$request['liitostunnus']}'";
	}

	if ($request['toim'] == 'TEHDYT_TYOT') {
		$lasku_where .= "	AND lasku.tila = 'L'
							AND lasku.alatila = 'D'";
	}
	else {
		$lasku_where .= "	AND lasku.tila = 'A'";
	}

	if (!empty($request['toimitusaika_haku'])) {
		$lasku_where .= "	AND lasku.toimaika < DATE_ADD(NOW(), INTERVAL {$request['toimitusaika_haku']} DAY)";
	}

	$query = "	SELECT
				lasku.tunnus as lasku_tunnus,
				lasku.ytunnus as asiakas_ytunnus,
				lasku.nimi as asiakas_nimi,
				kohde.tunnus as kohde_tunnus,
				kohde.nimi as kohde_nimi,
				paikka.nimi as paikka_nimi,
				a3.selitetark as paikka_olosuhde,
				laite.tuoteno,
				laite.oma_numero,
				laite.sarjanro as sarjanumero,
				laite.sijainti as laite_sijainti,
				lasku.toimaika,
				a1.selite as tyojonokoodi,
				a1.selitetark as tyojono,
				a2.selite as tyostatus_koodi,
				a2.selitetark as tyostatus,
				a2.selitetark_2 as tyostatusvari,
				tilausrivi.tunnus as tilausrivi_tunnus,
				tilausrivi.nimitys as tehtava_tyo,
				tilausrivin_lisatiedot.tilausrivilinkki as poikkeus_tunnus,
				IF(tilausrivin_lisatiedot.tilausrivilinkki != 0, concat_ws(' -> ', poikkeus_rivi.nimitys, tilausrivi.nimitys), '') as poikkeus_teksti,
				poistettu_laite.tuoteno as poistettu_laite
				FROM lasku
				JOIN tyomaarays
				ON ( tyomaarays.yhtio = lasku.yhtio
					AND tyomaarays.otunnus = lasku.tunnus )
				JOIN laskun_lisatiedot
				ON ( laskun_lisatiedot.yhtio = lasku.yhtio
					AND laskun_lisatiedot.otunnus = lasku.tunnus )
				JOIN tilausrivi
				ON ( tilausrivi.yhtio = lasku.yhtio
					AND tilausrivi.otunnus = lasku.tunnus
					AND tilausrivi.var != 'P' )
				JOIN tilausrivin_lisatiedot
				ON ( tilausrivin_lisatiedot.yhtio = tilausrivi.yhtio
					AND tilausrivin_lisatiedot.tilausrivitunnus = tilausrivi.tunnus )
				LEFT JOIN tilausrivi as poikkeus_rivi
				ON ( poikkeus_rivi.yhtio = tilausrivin_lisatiedot.yhtio
					AND poikkeus_rivi.tunnus = tilausrivin_lisatiedot.tilausrivilinkki )
				LEFT JOIN tilausrivin_lisatiedot as poikkeus_toimenpide_tilausrivi_lisatiedot
				ON ( poikkeus_toimenpide_tilausrivi_lisatiedot.yhtio = poikkeus_rivi.yhtio
					AND poikkeus_toimenpide_tilausrivi_lisatiedot.tilausrivitunnus = poikkeus_rivi.tunnus )
				LEFT JOIN laite as poistettu_laite
				ON ( poistettu_laite.yhtio = poikkeus_toimenpide_tilausrivi_lisatiedot.yhtio
					AND poistettu_laite.tunnus = poikkeus_toimenpide_tilausrivi_lisatiedot.asiakkaan_positio )
				JOIN laite
				ON ( laite.yhtio = tilausrivin_lisatiedot.yhtio
					AND laite.tunnus = tilausrivin_lisatiedot.asiakkaan_positio )
				JOIN paikka
				ON ( paikka.yhtio = laite.yhtio
					AND paikka.tunnus = laite.paikka )
				JOIN avainsana a3
				ON ( a3.yhtio = paikka.yhtio
					AND a3.selite = paikka.olosuhde
					AND a3.laji = 'OLOSUHDE')
				JOIN kohde
				ON ( kohde.yhtio = paikka.yhtio
					AND kohde.tunnus = paikka.kohde )
				LEFT JOIN avainsana a1
				ON ( a1.yhtio = tyomaarays.yhtio
					AND a1.laji = 'TYOM_TYOJONO'
					AND a1.selite = tyomaarays.tyojono )
				LEFT JOIN avainsana a2
				ON ( a2.yhtio = tyomaarays.yhtio
					AND a2.laji = 'TYOM_TYOSTATUS'
					AND a2.selite = tyomaarays.tyostatus )
				WHERE lasku.yhtio = '{$kukarow['yhtio']}'
				{$lasku_where}";
	$result = pupe_query($query);

	$tyomaaraykset = array();
	while ($tyomaarays = mysql_fetch_assoc($result)) {
		$tyomaaraykset[] = $tyomaarays;
	}

	return $tyomaaraykset;
}

function kasittele_tyomaaraykset($request) {
	global $kukarow, $yhtiorow;

	$tyomaaraykset_temp = array();
	foreach ($request['tyomaaraykset'] as $tyomaarays) {

		if (!empty($tyomaarays['poistettu_laite'])) {
			$tyomaarays['poikkeus_teksti'] .= '<br/>'.$tyomaarays['poistettu_laite'].' -> '.$tyomaarays['tuoteno'];
		}

		if (!isset($tyomaaraykset_temp[$tyomaarays['kohde_tunnus']])) {
			$tyomaaraykset_temp[$tyomaarays['kohde_tunnus']] = array(
				'asiakas_ytunnus'	 => $tyomaarays['asiakas_ytunnus'],
				'asiakas_nimi'		 => $tyomaarays['asiakas_nimi'],
				'kohde_tunnus'		 => $tyomaarays['kohde_tunnus'],
				'kohde_nimi'		 => $tyomaarays['kohde_nimi'],
			);
		}

		$tyomaaraykset_temp[$tyomaarays['kohde_tunnus']]['toimajat'][] = $tyomaarays['toimaika'];

		//jos kohteen kaikilla ty�m��r�yksill� on sama ty�status, asetetaan se my�s kohteelle, jotta se voidaan n�ytt�� ylemm�ll� tasolla taulukko n�kym�ss�
		if (isset($tyomaaraykset_temp[$tyomaarays['kohde_tunnus']]['tyostatus'])) {
			if ($tyomaaraykset_temp[$tyomaarays['kohde_tunnus']]['tyostatus'] != $tyomaarays['tyostatus']) {
				$tyomaaraykset_temp[$tyomaarays['kohde_tunnus']]['tyostatus'] = '';
				$tyomaaraykset_temp[$tyomaarays['kohde_tunnus']]['tyostatus_koodi'] = '';
				$tyomaaraykset_temp[$tyomaarays['kohde_tunnus']]['tyostatusvari'] = '';
			}
			else {
				$tyomaaraykset_temp[$tyomaarays['kohde_tunnus']]['tyostatus'] = $tyomaarays['tyostatus'];
				$tyomaaraykset_temp[$tyomaarays['kohde_tunnus']]['tyostatus_koodi'] = $tyomaarays['tyostatus_koodi'];
				$tyomaaraykset_temp[$tyomaarays['kohde_tunnus']]['tyostatusvari'] = $tyomaarays['tyostatusvari'];
			}
		}
		else {
			$tyomaaraykset_temp[$tyomaarays['kohde_tunnus']]['tyostatus'] = $tyomaarays['tyostatus'];
			$tyomaaraykset_temp[$tyomaarays['kohde_tunnus']]['tyostatus_koodi'] = $tyomaarays['tyostatus_koodi'];
			$tyomaaraykset_temp[$tyomaarays['kohde_tunnus']]['tyostatusvari'] = $tyomaarays['tyostatusvari'];
		}

		//jos kohteen kaikilla ty�m��r�yksill� on sama ty�jono, asetetaan se my�s kohteelle, jotta se voidaan n�ytt�� ylemm�ll� tasolla taulukko n�kym�ss�
		if (isset($tyomaaraykset_temp[$tyomaarays['kohde_tunnus']]['tyojonokoodi'])) {
			if ($tyomaaraykset_temp[$tyomaarays['kohde_tunnus']]['tyojonokoodi'] != $tyomaarays['tyojonokoodi']) {
				$tyomaaraykset_temp[$tyomaarays['kohde_tunnus']]['tyojonokoodi'] = '';
				$tyomaaraykset_temp[$tyomaarays['kohde_tunnus']]['tyojono'] = '';
			}
			else {
				$tyomaaraykset_temp[$tyomaarays['kohde_tunnus']]['tyojonokoodi'] = $tyomaarays['tyojonokoodi'];
				$tyomaaraykset_temp[$tyomaarays['kohde_tunnus']]['tyojono'] = $tyomaarays['tyojono'];
			}
		}
		else {
			$tyomaaraykset_temp[$tyomaarays['kohde_tunnus']]['tyojonokoodi'] = $tyomaarays['tyojonokoodi'];
			$tyomaaraykset_temp[$tyomaarays['kohde_tunnus']]['tyojono'] = $tyomaarays['tyojono'];
		}

		$tyomaaraykset_temp[$tyomaarays['kohde_tunnus']]['lasku_tunnukset'][] = $tyomaarays['lasku_tunnus'];
		$tyomaaraykset_temp[$tyomaarays['kohde_tunnus']]['tyomaaraykset'][] = $tyomaarays;
	}

	return $tyomaaraykset_temp;
}

function aseta_tyomaaraysten_status($lasku_tunnukset, $status) {
	global $kukarow, $yhtiorow;

	$query = "	UPDATE tyomaarays
				SET tyostatus = '{$status}'
				WHERE yhtio = '{$kukarow['yhtio']}'
				AND tyomaarays.otunnus IN ('".implode("','", $lasku_tunnukset)."')";
	pupe_query($query);
}

function merkkaa_tyomaarays_tehdyksi($request) {
	global $kukarow, $yhtiorow;

	$query = "	UPDATE lasku
				SET tila = 'L',
				alatila = 'D'
				WHERE yhtio = '{$kukarow['yhtio']}'
				AND tunnus IN ('".implode("','", $request['lasku_tunnukset'])."')";
	pupe_query($query);

	//Toimitettuaika manual setti� k�ytet��n konversio-ajossa
	if (empty($request['toimitettuaika'])) {
		$toimitettuaika_update = "toimitettuaika = NOW()";
	}
	else {
		$toimitettuaika_update = "toimitettuaika = '{$request['toimitettuaika']}'";
	}

	$query = "	UPDATE tilausrivi
				SET toimitettu = '{$kukarow['kuka']}',
				{$toimitettuaika_update}
				WHERE yhtio = '{$kukarow['yhtio']}'
				AND otunnus IN ('".implode("','", $request['lasku_tunnukset'])."')";
	pupe_query($query);

	aseta_tyomaaraysten_status($request['lasku_tunnukset'], 'X');
}

function echo_tyomaaraykset_table($request = array()) {
	global $kukarow, $yhtiorow, $palvelin2;

	$toimitusaika_haku_array = array(
		''	 => t('N�yt� kaikki'),
		7	 => t('1 viikon p��st� er��ntyv�t'),
		14	 => t('2 viikon p��st� er��ntyv�t'),
		21	 => t('3 viikon p��st� er��ntyv�t'),
		28	 => t('4 viikon p��st� er��ntyv�t'),
	);

	echo "<table class='display dataTable'>";
	echo "<thead>";

	$toimitus_string = "Toimitetaan";
	if ($request['toim'] == 'TEHDYT_TYOT') {
		$toimitus_string = "Toimitettu";
	}
	echo "<tr>
			<th>".t("Ty�m").".<br>".t("Viite")."</th>
			<th>".t("Ytunnus")."<br>".t("Asiakas")."</th>
			<th>".t("Kohde")."</th>
			<th>".t($toimitus_string)."</th>
			<th>".t("Ty�jono")."/<br>".t("Ty�status")."</th>
			<th></th>
		</tr>";

	echo "<tr>";

	echo "<td valign='top'><input type='text' size='10' name='tyomaarays_haku'></td>";

	echo "<td valign='top'><input type='text' size='10' name='asiakasnimi_haku'></td>";
	echo "<td valign='top'><input type='text' size='10' name='kohde_haku'></td>";
	echo "<td valign='top'>";
	echo "<form name='' method='POST' action=''>";
	echo "<select id='toimitusaika_select' name='toimitusaika_haku' onchange='this.form.submit()' >";
	foreach ($toimitusaika_haku_array as $toimitusaika_value => $toimitusaika) {
		$sel = "";
		if ($toimitusaika_value == $request['toimitusaika_haku']) {
			$sel = "SELECTED";
		}
		echo "<option value='{$toimitusaika_value}' {$sel}>{$toimitusaika}</option>";
	}
	echo "</select>";
	echo "</form>";
	echo "</td>";

	echo "<td>";

	echo "<select class='tyojono_sort'>";
	foreach ($request['tyojonot'] as $tyojono) {
		echo "<option value='{$tyojono['value']}'>{$tyojono['text']}</option>";
	}
	echo "</select>";
	echo "<br/>";
	echo "<select class='tyostatus_sort'>";
	foreach ($request['tyostatukset'] as $tyostatus) {
		echo "<option value='{$tyostatus['value']}'>{$tyostatus['text']}</option>";
	}
	echo "</select>";
	echo "</td>";

	echo "<td>";
	echo "</td>";

	echo "</tr>";

	echo "</thead>";

	echo "<tbody>";

	foreach ($request['tyomaaraykset'] as $tyomaarays) {
		echo "<tr class='kohde_tr_hidden aktiivi'>";

		echo "<td>";
		echo count($tyomaarays['lasku_tunnukset']);
		echo "</td>";

		echo "<td>";
		echo $tyomaarays['asiakas_ytunnus'].'<br/>'.$tyomaarays['asiakas_nimi'];
		echo "</td>";

		echo "<td>";
		echo $tyomaarays['kohde_nimi'];
		echo "</td>";

		echo "<td>";
		echo implode('<br/>', $tyomaarays['toimajat']);
		echo "</td>";

		if ($request['toim'] == 'TEHDYT_TYOT') {
			echo "<td>";
			echo $tyomaarays['tyojono'];
			echo "<br/>";
			echo $tyomaarays['tyostatus'];
			echo "</td>";

			echo "<td>";
			echo "<form class='multisubmit' name='tulosta_tarkastuspoytakirja'>";
			echo "<input type='hidden' name='ala_tee' value='tulosta_tarkastuspoytakirja' />";
			echo "<input type='hidden' name='toim' value='{$request['toim']}' />";
			echo "<input type='hidden' name='lasku_tunnukset' value='".implode(',', $tyomaarays['lasku_tunnukset'])."' />";
			echo "<input type='submit' value='".t("Tallenna tarkastuspy�t�kirja PDF")."' />";
			echo "</form>";
			echo "</td>";
		}
		else {
			if (!empty($tyomaarays["tyostatusvari"])) {
				$td_taustavari = "style='background-color: {$tyomaarays['tyostatusvari']};'";
			}
			else {
				$td_taustavari = "";
			}
			echo "<td {$td_taustavari}>";

			echo "<select class='tyojono_muutos'>";
			foreach ($request['tyojonot'] as $tyojono) {
				$sel = $tyomaarays['tyojonokoodi'] == $tyojono['value'] ? ' SELECTED' : '';
				echo "<option value='{$tyojono['value']}' {$sel}>{$tyojono['text']}</option>";
			}
			echo "</select>";
			echo "<br/>";
			echo "<select class='tyostatus_muutos'>";
			foreach ($request['tyostatukset'] as $tyostatus) {
				$sel = $tyomaarays['tyostatus_koodi'] == $tyostatus['value'] ? ' SELECTED' : '';
				echo "<option value='{$tyostatus['value']}' {$sel}>{$tyostatus['text']}</option>";
			}
			echo "</select>";

			echo "</td>";

			echo "<td>";
			echo "<form class='multisubmit' name='tulosta_tyolista'>";
			echo "<input type='hidden' name='ala_tee' value='tulosta_tyolista' />";
			echo "<input type='hidden' name='lasku_tunnukset' value='".implode(',', $tyomaarays['lasku_tunnukset'])."' />";
			echo "<input type='submit' value='".t("Tallenna ty�lista PDF")."' />";
			echo "</form>";
			echo "</td>";
		}

		echo "</tr>";

		echo_tyomaarays_rivi($request, $tyomaarays['tyomaaraykset']);
	}

	if (empty($request['tyomaaraykset'])) {
		echo "<tr>";
		echo "<td colspan = '6' align='center'>".t('Ei ty�m��r�yksi�')."</td>";
		echo "</tr>";
	}

	echo "</tbody>";
	echo "</table>";
}

function echo_tyomaarays_rivi($request, $laitteet) {
	global $kukarow, $yhtiorow, $palvelin2;

	echo "<tr class='laite_table_tr_hidden'>";

	echo "<form class='multisubmit' name='merkkaa_tehdyksi'>";
	echo "<input type='hidden' name='ala_tee' value='merkkaa_tehdyksi' />";

	echo "<td colspan='1'>";
	echo "</td>";
	echo "<td colspan='4'>";

	echo "<table class='laite_table'>";
	echo "<tr>";
	echo "<th>";
	echo t("#");
	if ($request['toim'] != 'TEHDYT_TYOT') {
		echo "<br/>";
		echo "<input type='checkbox' class='select_kaikki' />";
	}
	echo "</th>";
	echo "<th>".t("Oma numero")."</th>";
	echo "<th>".t("Sarjanumero")."</th>";
	echo "<th>".t("Laite")."</th>";
	echo "<th>".t("Paikka")."</th>";
	echo "<th>".t("Sijainti")."</th>";
	echo "<th>".t("Teht�v� ty�")."</th>";
	echo "<th>".t("Poikkeus")."</th>";
	echo "<th>".t("Ty�jono")."/<br>".t("Ty�status")."</th>";
	echo "<th>".t("Poikkeama")."</th>";
	echo "</tr>";

	foreach ($laitteet as $laite) {
		echo "<tr class='laite_tr'>";

		echo "<td>";
		if ($request['toim'] != 'TEHDYT_TYOT') {
			echo "<input type='checkbox' class='laite_checkbox' />";
		}
		echo "<input type='hidden' class='lasku_tunnus' value='{$laite['lasku_tunnus']}' />";
		echo "</td>";

		echo "<td>";
		echo $laite['oma_numero'];
		echo "</td>";

		echo "<td>";
		echo $laite['sarjanumero'];
		echo "</td>";

		echo "<td>";
		echo $laite['tuoteno'];
		echo "</td>";

		echo "<td>";
		echo $laite['paikka_nimi'];
		echo "</td>";

		echo "<td>";
		echo $laite['laite_sijainti'];
		echo "</td>";

		echo "<td>";
		echo $laite['tehtava_tyo'];
		echo "</td>";

		echo "<td>";
		echo "<font class='error'>{$laite['poikkeus_teksti']}</font>";
		echo "</td>";

		if ($request['toim'] == 'TEHDYT_TYOT') {
			echo "<td>";
			echo $laite['tyojono'];
			echo "<br/>";
			echo $laite['tyojono'];
			echo "</td>";

			echo "<td>";
			if ($laite['poikkeus_tunnus']) {
				echo "<font class='error'>".t("Poikkeus")."</font>";
				echo "<br/>";
				echo "<form class='multisubmit' name='tulosta_poikkeamaraportti' method='POST'>";
				echo "<input type='hidden' name='toim' value='{$request['toim']}' />";
				echo "<input type='hidden' name='ala_tee' value='tulosta_poikkeamaraportti' />";
				echo "<input type='hidden' name='lasku_tunnukset' value='{$laite['lasku_tunnus']}' />";
				echo "<input type='submit' value='".t('Tulosta poikkeamaraportti')."' />";
				echo "</form>";
			}
			echo "</td>";
		}
		else {
			if (!empty($laite["tyostatusvari"])) {
				$td_taustavari = "style='background-color: {$laite['tyostatusvari']};'";
			}
			else {
				$td_taustavari = "";
			}
			echo "<td {$td_taustavari}>";

			echo "<select class='tyojono_muutos'>";
			foreach ($request['tyojonot'] as $tyojono) {
				$sel = $laite['tyojono'] == $tyojono['selitetark'] ? ' SELECTED' : '';
				echo "<option value='{$tyojono['selite']}' {$sel}>{$tyojono['selitetark']}</option>";
			}
			echo "</select>";
			echo "<br/>";
			echo "<select class='tyostatus_muutos'>";
			foreach ($request['tyostatukset'] as $tyostatus) {
				$sel = $laite['tyostatus_koodi'] == $tyostatus['selite'] ? ' SELECTED' : '';
				echo "<option value='{$tyostatus['value']}' {$sel}>{$tyostatus['text']}</option>";
			}
			echo "</select>";

			echo "</td>";

			echo "<td>";
			//echo "<a href='{$palvelin2}asiakkaan_laite_hallinta.php?tee=hae_asiakas&ytunnus={$laite['asiakas_ytunnus']}&lopetus={$palvelin2}tyomaarays/tyojono2.php'><button type='button'>".t("Laitteen vaihto")."</button></a>";
			echo "<a href='{$palvelin2}tilauskasittely/laitteen_vaihto.php?tilausrivi_tunnus={$laite['tilausrivi_tunnus']}&lopetus={$palvelin2}tyomaarays/tyojono2.php'><button type='button'>".t("Laitteen vaihto")."</button></a>";
			echo "<br/>";
			echo "<a href='{$palvelin2}tilauskasittely/tilaus_myynti.php?toim=TYOMAARAYS&tilausnumero={$laite['lasku_tunnus']}&lopetus={$palvelin2}tyomaarays/tyojono2.php'><button type='button'>".t("Muu")."</button></a>";
			echo "</td>";
		}

		echo "</tr>";
	}

	echo "</table>";
	echo "</td>";

	if ($request['toim'] == 'TEHDYT_TYOT') {
		echo "<td>";
		echo "</td>";
	}
	else {
		echo "<td>";
		echo "<input type='submit' value='".t("Merkkaa tehdyksi")."' />";
		echo "</td>";
	}

	echo "</form>";
	echo "</tr>";
}

function hae_tyojonot($request = array()) {
	global $kukarow, $yhtiorow;

	$tyojono_result = t_avainsana("TYOM_TYOJONO");
	$tyojonot = array();
	$tyojonot[] = array(
		'value'	 => 'EIJONOA',
		'text'	 => t('Ei jonossa'),
	);
	while ($tyojono_row = mysql_fetch_assoc($tyojono_result)) {
		$tyojono_row['value'] = $tyojono_row['selite'];
		$tyojono_row['text'] = $tyojono_row['selitetark'];
		$tyojonot[] = $tyojono_row;
	}

	return $tyojonot;
}

function hae_tyostatukset($request = array()) {
	global $kukarow, $yhtiorow;

	$tyostatus_result = t_avainsana("TYOM_TYOSTATUS");
	$tyostatukset = array();
	$tyostatukset[] = array(
		'value'	 => 'EISTATUSTA',
		'text'	 => t('Ei statusta'),
	);
	while ($tyostatus_row = mysql_fetch_assoc($tyostatus_result)) {
		$tyostatus_row['value'] = $tyostatus_row['selite'];
		$tyostatus_row['text'] = $tyostatus_row['selitetark'];
		$tyostatukset[] = $tyostatus_row;
	}

	return $tyostatukset;
}

function hae_laitteen_viimeiset_tapahtumat($laite_tunnus) {
	global $kukarow, $yhtiorow;

	$query = "	SELECT huoltosyklit_laitteet.viimeinen_tapahtuma,
				tuotteen_avainsanat.selite AS tapahtuman_tyyppi
				FROM huoltosyklit_laitteet
				JOIN huoltosykli
				ON ( huoltosykli.yhtio = huoltosyklit_laitteet.yhtio
					AND huoltosykli.tunnus = huoltosyklit_laitteet.huoltosykli_tunnus )
				JOIN tuote as toimenpide_tuote
				ON ( toimenpide_tuote.yhtio = huoltosykli.yhtio
					AND toimenpide_tuote.tuoteno = huoltosykli.toimenpide )
				JOIN tuotteen_avainsanat
				ON ( tuotteen_avainsanat.yhtio = toimenpide_tuote.yhtio
					AND tuotteen_avainsanat.tuoteno = toimenpide_tuote.tuoteno
					AND tuotteen_avainsanat.laji = 'tyomaarayksen_ryhmittely')
				WHERE huoltosyklit_laitteet.yhtio = '{$kukarow['yhtio']}'
				AND huoltosyklit_laitteet.laite_tunnus = '{$laite_tunnus}'";
	$result = pupe_query($query);

	$viimeiset_tapahtumat = array();
	while($viimeinen_tapahtuma = mysql_fetch_assoc($result)) {
		$viimeiset_tapahtumat[$viimeinen_tapahtuma['tapahtuman_tyyppi']] = $viimeinen_tapahtuma['viimeinen_tapahtuma'];
	}

	return $viimeiset_tapahtumat;
}

function onko_laitteella_poikkeus($laite_tunnus) {
	global $kukarow, $yhtiorow;

	$query = "	SELECT *
				FROM tilausrivin_lisatiedot
				JOIN tilausrivi as poikkeus_rivi
				ON ( poikkeus_rivi.yhtio = tilausrivin_lisatiedot.yhtio
					AND poikkeus_rivi.tunnus = tilausrivin_lisatiedot.tilausrivilinkki )
				WHERE tilausrivin_lisatiedot.yhtio = '{$kukarow['yhtio']}'
				AND tilausrivin_lisatiedot.asiakkaan_positio = '{$laite_tunnus}'";
	$result = pupe_query($query);

	if (mysql_num_rows($result) > 0) {
		return true;
	}

	return false;
}