<?php

require('../inc/parametrit.inc');

require('inc/laitetarkista.inc');

//AJAX requestit t�nne
if ($ajax_request) {
	exit;
}

echo "<font class='head'>".t("Laitteen vaihto").":</font>";
echo "<hr/>";
echo "<br/>";
?>
<style>

</style>
<script>

</script>

<?php

$request = array(
	'tee'				 => $tee,
	'tilausrivi_tunnus'	 => $tilausrivi_tunnus,
	'lasku_tunnus'		 => $lasku_tunnus,
	'uusi_laite'		 => $uusi_laite,
	'vanha_laite_tunnus' => $vanha_laite_tunnus,
);

$request['laite'] = hae_laite_ja_asiakastiedot($request['tilausrivi_tunnus']);
$request['paikat'] = hae_paikat();

if (!empty($request['uusi_laite'])) {
	if (!empty($request['uusi_laite']['varalaite'])) {
		$request['uusi_laite']['tila'] = 'V';
	}
	else {
		$request['uusi_laite']['tila'] = 'N';
	}
	unset($request['uusi_laite']['varalaite']);

	$request['virheet'] = validoi_uusi_laite($request['uusi_laite']);

	if (!empty($request['virheet'])) {
		$request['tee'] = '';
	}
}

if ($request['tee'] == 'vaihda_laite') {
	//luo_uusi_laite funktio liitt�� my�s laitteen oikealle paikalle asiakkaan laite puuhun
	$request['uusi_laite'] = luo_uusi_laite($request['uusi_laite']);

	//vanha ty�m��r�ys rivi pit�� asettaa var P tilaan,
	//jotta tekem�t�n ty�m��r�ys rivi ei mene laskutukseen
	//ja tyomaarayksen status Laite vaihdettu tilaan, jotta k�ytt�liittym� on selke�mpi
	aseta_tyomaarays_var($request['tilausrivi_tunnus'], 'P');
	aseta_tyomaarays_status($request['tilausrivi_tunnus'], 'V');

	//pit�� luoda uusi ty�m��r�ys rivi johon tulee laitteen vaihto tuote tjsp
	$request['vaihto_toimenpide'] = hae_vaihtotoimenpide_tuote();
	$request['vaihto_toimenpide_tyomaarays_tilausrivi_tunnus'] = luo_uusi_tyomaarays_rivi($request['vaihto_toimenpide'], $request['lasku_tunnus']);
	aseta_uuden_tyomaarays_rivin_kommentti($request['vaihto_toimenpide_tyomaarays_tilausrivi_tunnus'], $request['uusi_laite']['kommentti']);

	//vaihtotoimenpide ja uusi laite pit�� linkata toisiinsa
	linkkaa_uusi_laite_vaihtotoimenpiteeseen($request['uusi_laite'], $request['vaihto_toimenpide_tyomaarays_tilausrivi_tunnus']);

	//tekem�tt�m�st� ty�m��r�ys rivist� ei nollata h�vinneen laitteen tunnusta.
	//Tekem�t�n ty�m��r�ysrivi ja h�vinnyt laite asetetaan poistettu tilaan, joten unlinkkaus ei ole tarpeellinen
	//nollaa_vanhan_toimenpiderivin_asiakas_positio($request['tilausrivi_tunnus']);
	//ty�m��r�ykselle pit�� liitt�� uusi laite.
	//HUOM uudesta laitteesta ei ehk� aina haluta luoda uutta ty�m��r�ys rivi�,
	//jos esim laitetta ei myyd� asiakkaalle vaan se menee vain lainaan
	if ($request['uusi_laite']['tila'] == 'N') {
		$request['uusi_laite_tyomaarays_tilausrivi_tunnus'] = luo_uusi_tyomaarays_rivi($request['uusi_laite'], $request['lasku_tunnus']);

		//ty�m��r�ykselle lis�t��n my�s uusi laite. t�h�n rivin pit�� linkata vanha toimenpide tilausrivi
		paivita_uuden_toimenpide_rivin_tilausrivi_linkki($request['uusi_laite_tyomaarays_tilausrivi_tunnus'], $request['tilausrivi_tunnus']);
	}

	//kun toimenpide vaihtuu ty�m��r�yksell� niin vanha toimenpide laitetaan P tilaan (ylemp�n�)
	//ja vanha toimenpide linkataan uuteen toimenpide riviin tilausrivin_lisatiedot.tilausrivilinkki kent�n avulla,
	//jotta raportit osaavat n�ytt�� mik� toimenpide vaihdettiin mihin.
	paivita_uuden_toimenpide_rivin_tilausrivi_linkki($request['vaihto_toimenpide_tyomaarays_tilausrivi_tunnus'], $request['tilausrivi_tunnus']);

    if ($request['uusi_laite']['tila'] == 'N') {
        //Jos uusi laite on normaali laite asetetaan vanha laite poistettu tilaan
        aseta_laitteen_tila($request['vanha_laite_tunnus'], 'P');
    }
    else if ($request['uusi_laite']['tila'] == 'V'){
        //Jos laite on varalaite asetetaan vanha laite huollossa tilaan
        aseta_laitteen_tila($request['vanha_laite_tunnus'], 'H');
    }
    else {
        //defaulttina asetetaan vanha laite poistettu tilaan
        aseta_laitteen_tila($request['vanha_laite_tunnus'], 'P');
    }
	

	echo '<font class="message">'.t("Laite vaihdettu").'</font>';
	echo "<br/>";
	echo "<br/>";

	//vanha laite on h�vinnyt/mennyt rikki. k�yd��n vanhan laitteen ty�m��r�ysrivit l�pi, ja asetetaan ne poistettu tilaan.
	$poistetut_tilaukset = aseta_vanhan_laitteen_tyomaarays_rivit_poistettu_tilaan($request['vanha_laite_tunnus']);

	if (!empty($poistetut_tilaukset)) {
		$poistetut_tilaukset = implode(', ', $poistetut_tilaukset);
		$poistetut_tilaukset = substr($poistetut_tilaukset, 0, -2);
		echo t('Seuraavat ty�m��r�ykset poistettiin, koska niihin liitetty laite on kadonnut/hajonnut').': '.$poistetut_tilaukset;
	}
	else {
		echo t('Laitteella ei ollut muita poistettavia ty�m��r�yksi�');
	}
}
else {
	echo_laitteen_vaihto_form($request);
}

require('inc/footer.inc');

function validoi_uusi_laite($uusi_laite) {
	global $kukarow, $yhtiorow;

	$virhe = array();

	$query = "	SELECT *
				FROM laite
				WHERE yhtio = '{$kukarow['yhtio']}'
				AND tunnus = ''";
	$result = pupe_query($query);
	$trow = mysql_fetch_assoc($result);

	for ($i = 1; $i < mysql_num_fields($result); $i++) {
		laitetarkista($uusi_laite, $i, $result, '', $virhe, $trow);
	}

	return $virhe;
}

function luo_uusi_laite($uusi_laite) {
	global $kukarow, $yhtiorow;

	//t�m�n funktion j�lkeen ajetaan aseta_uuden_tyomaarays_rivin_kommentti.
	//alla oleva unset ei vaikuta siihen
	unset($uusi_laite['kommentti']);

	$uusi_laite['yhtio'] = $kukarow['yhtio'];
	$query = "INSERT INTO
				laite (".implode(", ", array_keys($uusi_laite)).")
				VALUES('".implode("', '", array_values($uusi_laite))."')";
	pupe_query($query);

	$query = "	SELECT laite.*,
				tuote.nimitys as tuote_nimitys,
				tuote.myyntihinta as tuote_hinta,
				tuote.try as tuote_try
				FROM laite
				JOIN tuote
				ON ( tuote.yhtio = laite.yhtio
					AND tuote.tuoteno = laite.tuoteno )
				WHERE laite.yhtio = '{$kukarow['yhtio']}'
				AND laite.tunnus = '".mysql_insert_id()."'";
	$result = pupe_query($query);

	return mysql_fetch_assoc($result);
}

function aseta_tyomaarays_var($tilausrivi_tunnus, $var) {
	global $kukarow, $yhtiorow;

	$query = "	UPDATE tilausrivi
				SET var = '{$var}'
				WHERE yhtio = '{$kukarow['yhtio']}'
				AND tunnus = '{$tilausrivi_tunnus}'";
	pupe_query($query);
}

function aseta_tyomaarays_status($tilausrivi_tunnus, $status) {
	global $kukarow, $yhtiorow;

	$query = "	UPDATE tyomaarays
				JOIN lasku
				ON ( lasku.yhtio = tyomaarays.yhtio
					AND lasku.tunnus = tyomaarays.otunnus )
				JOIN tilausrivi
				ON ( tilausrivi.yhtio = lasku.yhtio
					AND tilausrivi.otunnus = lasku.tunnus
					AND tilausrivi.tunnus = '{$tilausrivi_tunnus}')
				SET tyomaarays.tyostatus = '{$status}'
				WHERE tyomaarays.yhtio = '{$kukarow['yhtio']}'";
	pupe_query($query);
}

function aseta_uuden_tyomaarays_rivin_kommentti($tilausrivi_tunnus, $kommentti) {
	global $kukarow, $yhtiorow;

	$query = "	UPDATE tilausrivi
				SET kommentti = '{$kommentti}'
				WHERE yhtio = '{$kukarow['yhtio']}'
				AND tunnus = {$tilausrivi_tunnus}";
	pupe_query($query);
}

function luo_uusi_tyomaarays_rivi($tuote, $lasku_tunnus) {
	global $kukarow, $yhtiorow;

	$query = "	INSERT INTO tilausrivi
				SET hyllyalue = '',
				hyllynro = '',
				hyllyvali = '',
				hyllytaso = '',
				tilaajanrivinro = '',
				laatija = '{$kukarow['kuka']}',
				laadittu = now(),
				yhtio = '{$kukarow['yhtio']}',
				tuoteno = '{$tuote['tuoteno']}',
				varattu = '0',
				yksikko = '',
				kpl = '0',
				kpl2 = '',
				tilkpl = '1',
				jt = '1',
				ale1 = '0',
				erikoisale = '0.00',
				alv = '0',
				netto = '',
				hinta = '{$tuote['hinta']}',
				kerayspvm = CURRENT_DATE,
				otunnus = '{$lasku_tunnus}',
				tyyppi = 'L',
				toimaika = CURRENT_DATE,
				kommentti = '',
				var = 'J',
				try= '{$tuote['try']}',
				osasto = '0',
				perheid = '0',
				perheid2 = '0',
				tunnus = '0',
				nimitys = '{$tuote['tuote_nimitys']}',
				jaksotettu = ''";
	pupe_query($query);

	$tilausrivi_tunnus = mysql_insert_id();

	$query = "	INSERT INTO tilausrivin_lisatiedot
				SET yhtio = '{$kukarow['yhtio']}',
				positio = '',
				toimittajan_tunnus = '',
				tilausrivitunnus = '{$tilausrivi_tunnus}',
				jarjestys = '0',
				vanha_otunnus = '{$lasku_tunnus}',
				ei_nayteta = '',
				ohita_kerays = '',
				luontiaika = now(),
				laatija = '{$kukarow['kuka']}'";
	pupe_query($query);

	return $tilausrivi_tunnus;
}

function hae_laite_ja_asiakastiedot($tilausrivi_tunnus) {
	global $kukarow, $yhtiorow;

	$query = "	SELECT laite.*,
				laite.tunnus as laite_tunnus,
				tilausrivi.tunnus as tilausrivi_tunnus,
				paikka.nimi as paikka_nimi,
				paikka.tunnus as paikka_tunnus,
				kohde.nimi as kohde_nimi,
				asiakas.nimi as asiakas_nimi,
				lasku.tunnus as lasku_tunnus
				FROM tilausrivi
				JOIN tilausrivin_lisatiedot
				ON ( tilausrivin_lisatiedot.yhtio = tilausrivi.yhtio
					AND tilausrivin_lisatiedot.tilausrivitunnus = tilausrivi.tunnus )
				JOIN lasku
				ON ( lasku.yhtio = tilausrivi.yhtio
					AND lasku.tunnus = tilausrivi.otunnus )
				JOIN laite
				ON ( laite.yhtio = tilausrivin_lisatiedot.yhtio
					AND laite.tunnus = tilausrivin_lisatiedot.asiakkaan_positio )
				JOIN paikka
				ON ( paikka.yhtio = laite.yhtio
					AND paikka.tunnus = laite.paikka )
				JOIN kohde
				ON ( kohde.yhtio = paikka.yhtio
					AND kohde.tunnus = paikka.kohde )
				JOIN asiakas
				ON ( asiakas.yhtio = kohde.yhtio
					AND asiakas.tunnus = kohde.asiakas )
				WHERE tilausrivi.yhtio = '{$kukarow['yhtio']}'
				AND tilausrivi.tunnus = '{$tilausrivi_tunnus}'";
	$result = pupe_query($query);

	return mysql_fetch_assoc($result);
}

function hae_vaihtotoimenpide_tuote() {
	global $kukarow, $yhtiorow;

	$query = "	SELECT tuote.*,
				tuote.nimitys as tuote_nimitys
				FROM tuote
				WHERE yhtio = '{$kukarow['yhtio']}'
				AND tuoteno = 'LAITTEEN_VAIHTO'";
	$result = pupe_query($query);

	return mysql_fetch_assoc($result);
}

function linkkaa_uusi_laite_vaihtotoimenpiteeseen($uusi_laite, $tilausrivi_tunnus) {
	global $kukarow, $yhtiorow;

	$query = "	UPDATE tilausrivi
				JOIN tilausrivin_lisatiedot
				ON ( tilausrivin_lisatiedot.yhtio = tilausrivi.yhtio
					AND tilausrivin_lisatiedot.tilausrivitunnus = tilausrivi.tunnus )
				SET tilausrivin_lisatiedot.asiakkaan_positio = '{$uusi_laite['tunnus']}'
				WHERE tilausrivi.yhtio = '{$kukarow['yhtio']}'
				AND tilausrivi.tunnus = '{$tilausrivi_tunnus}'";
	pupe_query($query);
}

function nollaa_vanhan_toimenpiderivin_asiakas_positio($tilausrivi_tunnus) {
	global $kukarow, $yhtiorow;

	$query = "	UPDATE tilausrivi
				JOIN tilausrivin_lisatiedot
				ON ( tilausrivin_lisatiedot.yhtio = tilausrivi.yhtio
					AND tilausrivin_lisatiedot.tilausrivitunnus = tilausrivi.tunnus )
				SET tilausrivin_lisatiedot.asiakkaan_positio = '0'
				WHERE tilausrivi.yhtio = '{$kukarow['yhtio']}'
				AND tilausrivi.tunnus = '{$tilausrivi_tunnus}'";
	pupe_query($query);
}

function paivita_uuden_toimenpide_rivin_tilausrivi_linkki($toimenpide_tilausrivi_tunnus, $vanha_tilausrivi_tunnus) {
	global $kukarow, $yhtiorow;

	$query = "	UPDATE tilausrivi
				JOIN tilausrivin_lisatiedot
				ON ( tilausrivin_lisatiedot.yhtio = tilausrivi.yhtio
					AND tilausrivin_lisatiedot.tilausrivitunnus = tilausrivi.tunnus )
				SET tilausrivin_lisatiedot.tilausrivilinkki = '{$vanha_tilausrivi_tunnus}'
				WHERE tilausrivi.yhtio = '{$kukarow['yhtio']}'
				AND tilausrivi.tunnus = '{$toimenpide_tilausrivi_tunnus}'";
	pupe_query($query);
}

function hae_paikat() {
	global $kukarow, $yhtiorow;

	$query = "	SELECT *
				FROM paikka
				WHERE yhtio = '{$kukarow['yhtio']}'";
	$result = pupe_query($query);

	$paikat = array();
	while ($paikka = mysql_fetch_assoc($result)) {
		$paikat[] = $paikka;
	}

	return $paikat;
}

function aseta_laitteen_tila($laite_tunnus, $tila) {
	global $kukarow, $yhtiorow;

	$query = "	UPDATE laite
				SET tila = '{$tila}'
				WHERE yhtio = '{$kukarow['yhtio']}'
				AND tunnus = '{$laite_tunnus}'";
	pupe_query($query);
}

function aseta_vanhan_laitteen_tyomaarays_rivit_poistettu_tilaan($vanha_laite_tunnus) {
	global $kukarow, $yhtiorow;

	$query = "	SELECT tilausrivi.otunnus,
				tilausrivi.tunnus AS tilausrivi_tunnus
				FROM tilausrivi
				JOIN tilausrivin_lisatiedot
				ON ( tilausrivin_lisatiedot.yhtio = tilausrivi.yhtio
					AND tilausrivin_lisatiedot.tilausrivitunnus = tilausrivi.tunnus
					AND tilausrivin_lisatiedot.asiakkaan_positio = '{$vanha_laite_tunnus}' )
				WHERE tilausrivi.yhtio = '{$kukarow['yhtio']}'
				AND tilausrivi.var != 'P'";
	$result = pupe_query($query);

	$poistettavat_tilausrivit = array();
	$tilaukset = array();
	while ($poistettava_tilausrivi = mysql_fetch_assoc($result)) {
		$poistettavat_tilausrivit[] = $poistettava_tilausrivi['tilausrivi_tunnus'];
		$tilaukset[] = $poistettava_tilausrivi['otunnus'];
	}

	if (!empty($poistettavat_tilausrivit)) {
		$query = "	UPDATE tyomaarays
					JOIN lasku
					ON ( lasku.yhtio = tyomaarays.yhtio
						AND lasku.tunnus = tyomaarays.otunnus )
					JOIN tilausrivi
					ON ( tilausrivi.yhtio = lasku.yhtio
						AND tilausrivi.otunnus = lasku.tunnus
						AND tilausrivi.tunnus IN ('".implode("','", $poistettavat_tilausrivit)."') )
					SET tyomaarays.tyostatus = 'V',
					tilausrivi.var = 'P'
					WHERE tyomaarays.yhtio = '{$kukarow['yhtio']}'";
		pupe_query($query);
	}

	return $tilaukset;
}

function echo_laitteen_vaihto_form($request = array()) {
	global $kukarow, $yhtiorow, $oikeurow;

	if (!empty($request['virheet'])) {
		$virhe_message = implode('<br/>', $request['virheet']);

		echo '<font class="error">'.$virhe_message.'</font>';
		echo '<br/>';
	}

	echo "<form class='multisubmit' method='POST' action=''>";
	echo "<input type='hidden' name='tee' value='vaihda_laite' />";
	echo "<input type='hidden' name='tilausrivi_tunnus' value='{$request['laite']['tilausrivi_tunnus']}' />";
	echo "<input type='hidden' name='lasku_tunnus' value='{$request['laite']['lasku_tunnus']}' />";
	echo "<input type='hidden' name='vanha_laite_tunnus' value='{$request['laite']['laite_tunnus']}' />";
	echo "<table>";

	echo "<tr>";
	echo "<th>".t("Asiakas")."</th>";
	echo "<td>{$request['laite']['asiakas_nimi']}</td>";
	echo "</tr>";

	echo "<tr>";
	echo "<th>".t("Kohde")."</th>";
	echo "<td>{$request['laite']['kohde_nimi']}</td>";
	echo "</tr>";

	echo "<tr>";
	echo "<th>".t("Paikka")."</th>";
	echo "<td>{$request['laite']['paikka_nimi']}</td>";
	echo "</tr>";

	echo "<tr>";
	echo "<th>".t("Vaihdettava laite")."</th>";
	echo "<td>{$request['laite']['tuoteno']}</td>";
	echo "</tr>";

	echo "<tr>";
	echo "<th>".t("Uusi laite")."</th>";
	echo "<td>";
	echo '
	<table>
	<tbody>

	<tr>
	<th align="left">'.t("Tuotenumero").'</th>
	<td>
	<input type="text" name="uusi_laite[tuoteno]" value="'.$request['uusi_laite']['tuoteno'].'" size="35" maxlength="60" />
	</td>
	</tr>

	<tr>
	<th align="left">'.t("Sarjanumero").'</th>
	<td>
	<input type="text" name="uusi_laite[sarjanro]" value="'.$request['uusi_laite']['sarjanro'].'" size="35" maxlength="60" />
	</td>
	</tr>

	<tr>
	<th align="left">'.t("Valmistus p�iv�").'</th>
	<td>
	<input type="text" name="uusi_laite[valm_pvm]" value="'.$request['uusi_laite']['valm_pvm'].'" size="10" maxlength="10" />
	</td>
	</tr>

	<tr>
	<th align="left">'.t("Oma numero").'</th>
	<td>
	<input type="text" name="uusi_laite[oma_numero]" value="'.$request['uusi_laite']['oma_numero'].'" size="35" maxlength="20" />
	</td>
	</tr>

	<tr>
	<th align="left">'.t("Omistaja").'</th>
	<td>
	<input type="text" name="uusi_laite[omistaja]" value="'.$request['uusi_laite']['omistaja'].'" size="35" maxlength="60" />
	</td>
	</tr>

	<tr>
	<th align="left">'.t("Paikka").'</th>
	<td>';
	echo "<select name='uusi_laite[paikka]>";
	foreach ($request['paikat'] as $paikka) {
		$selected = ($paikka['tunnus'] == $request['laite']['paikka_tunnus']) ? 'SELECTED' : '';
		echo "<option value='{$paikka['tunnus']}' $selected>{$paikka['nimi']}</option>";
	}
	echo "</select>";
	echo '</td>
	</tr>

	<tr>
	<th align="left">'.t("Sijainti").'</th>
	<td>
	<input type="text" name="uusi_laite[sijainti]" value="'.$request['uusi_laite']['sijainti'].'" size="35" maxlength="60" />
	</td>
	</tr>

	<tr>
	<th align="left">'.t("Kommentti").'</th>
	<td>
	<input type="text" name="uusi_laite[kommentti]" value="'.$request['uusi_laite']['kommentti'].'" size="35" maxlength="60" />
	</td>
	</tr>

	<tr>
	<th align="left">'.t("Varalaite").'</th>
	<td>';
	$varalaite_chk = '';
	if (!empty($request['uusi_laite']['varalaite'])) {
		$varalaite_chk = 'CHECKED';
	}
	echo '<input type="checkbox" name="uusi_laite[varalaite]" '.$varalaite_chk.'/>
	</td>
	</tr>

	</tbody>
	</table>';
	echo "</td>";
	echo "</tr>";

	echo "</table>";

	echo "<input type='submit' value='".t("Vaihda laite")."' />";
	echo "</form>";
}