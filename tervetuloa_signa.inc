<?php

echo_myyntiseuranta_table();

function echo_myyntiseuranta_table() {
	global $kukarow, $yhtiorow;

	$nykyinen_kvartaali = paata_kvartaali();

	$tulos_ja_liikevaihto = hae_tulos_ja_liikevaihto($nykyinen_kvartaali);
	$avoimet_tilaukset = hae_avoimet_tilaukset($kukarow['tunnus']);

	if (stristr($kukarow['asema'], 'myyj�')) {
		$tilikausi = array(
			'start_date' => $yhtiorow['tilikausi_alku'],
			'end_date' => $yhtiorow['tilikausi_loppu'],
		);
		$myynti_ja_kate_nykyinen_tilikausi = hae_myyjan_myynti_ja_kate($tilikausi, $kukarow['tunnus']);
		$myynti_ja_kate_nykyinen_kvartaali = hae_myyjan_myynti_ja_kate($nykyinen_kvartaali, $kukarow['tunnus']);
		
	}

	echo "<table>";
	echo "<tr>";

	echo "<td class='back'>";

	echo "<table>";
	echo "<tr>";
	echo "<th colspan='2' style='text-align:center;'>{$yhtiorow['nimi']} <br/>".piirra_aikavali($nykyinen_kvartaali)."</th>";
	echo "</tr>";

	echo "<tr>";
	echo "<th>".t("Liikevaihto")."</th>";
	echo "<td align='right'>";
	echo sprintf('%.2f', $tulos_ja_liikevaihto['liikevaihto']) . " {$yhtiorow['valkoodi']}";
	echo "</td>";
	echo "</tr>";

	echo "<tr>";
	echo "<th>".t("Tulos")."</th>";
	echo "<td align='right'>";
	echo sprintf('%.2f', $tulos_ja_liikevaihto['tulos']) . " {$yhtiorow['valkoodi']}";
	echo "</td>";
	echo "</tr>";

	echo "</table>";

	echo "<br/>";

	echo "<table>";
	echo "<tr>";
	echo "<th style='text-align:center;' colspan='2'>".t("Avoimet tilaukset")."</th>";
	echo "</tr>";

	echo "<tr>";
	echo "<td align='right'>";

	echo sprintf('%.2f', $avoimet_tilaukset['arvo']) . " {$yhtiorow['valkoodi']}";
	echo "</td>";
	echo "</tr>";
	echo "</table>";

	echo "</td>";

	if (stristr($kukarow['asema'], 'myyj�')) {

		echo "<td class='back'>";

		echo "<table>";
		echo "<tr>";
		echo "<th style='text-align:center;' colspan='2'>{$kukarow['nimi']}</th>";
		echo "</tr>";


		echo "<tr>";
		echo "<td>";
		echo t("Liikevaihto").' '.piirra_aikavali($tilikausi);
		echo "</td>";
		echo "<td align='right'>";
		echo sprintf('%.2f', $myynti_ja_kate_nykyinen_tilikausi['myynti']) . " {$yhtiorow['valkoodi']}";
		echo "</td>";
		echo "</tr>";

		echo "<tr>";
		echo "<td>";
		echo t("Liikevaihto").' '.piirra_aikavali($nykyinen_kvartaali);
		echo "</td>";
		echo "<td align='right'>";
		echo sprintf('%.2f', $myynti_ja_kate_nykyinen_kvartaali['myynti']) . " {$yhtiorow['valkoodi']}";
		echo "</td>";
		echo "</tr>";

		echo "<tr>";
		echo "<td>";
		echo t("Kate").' '.piirra_aikavali($nykyinen_kvartaali);
		echo "</td>";
		echo "<td align='right'>";
		echo sprintf('%.2f', $myynti_ja_kate_nykyinen_kvartaali['kate']) . " {$yhtiorow['valkoodi']}";
		echo "</td>";
		echo "</tr>";

		echo "<tr>";
		echo "<td>";
		echo t("Avoimet tilaukset");
		echo "</td>";
		echo "<td align='right'>";
		echo sprintf('%.2f', $avoimet_tilaukset['myyjan_arvo']) . " {$yhtiorow['valkoodi']}";
		echo "</td>";
		echo "</tr>";
		echo "</table>";

		echo "</td>";
	}

	echo "</tr>";
	echo "</table>";
}

function hae_tulos_ja_liikevaihto($nykyinen_kvartaali) {
	global $kukarow;

	$query = "	SELECT
				IFNULL(round(Sum(if (tili.sisainen_taso != '3A', summa, 0)) * -1, 2), 0) AS tulos,
				IFNULL(round(Sum(if (tili.sisainen_taso LIKE '31%', summa, 0)) * -1, 2), 0) AS liikevaihto
				FROM tiliointi
				JOIN tili ON (tili.yhtio = tiliointi.yhtio
					AND tili.tilino = tiliointi.tilino
					AND tili.sisainen_taso LIKE '3%')
				WHERE tiliointi.yhtio = '{$kukarow['yhtio']}'
				AND tiliointi.korjattu = ''
				AND tiliointi.tapvm >= '{$nykyinen_kvartaali['start_date']}'
				AND tiliointi.tapvm <= '{$nykyinen_kvartaali['end_date']}'";
	$result = pupe_query($query);

	return mysql_fetch_assoc($result);
}

function hae_avoimet_tilaukset($kuka_tunnus) {
	global $kukarow, $yhtiorow;

	$query_ale_lisa = generoi_alekentta('M');
	$query_myyntihinta = "if(lasku.alatila='X', 0, tilausrivi.hinta / if('$yhtiorow[alv_kasittely]'  = '' and tilausrivi.alv < 500, (1+tilausrivi.alv/100), 1) * (tilausrivi.varattu+tilausrivi.jt) * {$query_ale_lisa})";

	$sumquery = "	SELECT
					IFNULL(round(sum($query_myyntihinta), 2), 0) arvo,
					IFNULL(round(sum(if(lasku.myyja = '$kuka_tunnus', $query_myyntihinta, 0)), 2), 0) myyjan_arvo
					FROM lasku use index (tila_index)
					JOIN tilausrivi use index (yhtio_otunnus) on (tilausrivi.yhtio = lasku.yhtio
						and tilausrivi.otunnus = lasku.tunnus
						and tilausrivi.tyyppi != 'D')
					WHERE lasku.yhtio = '$kukarow[yhtio]' 
					and lasku.tila in ('L', 'N')
					and lasku.alatila != 'X'";
	$sumresult = pupe_query($sumquery);

	return mysql_fetch_assoc($sumresult);
}

function hae_myyjan_myynti_ja_kate($aika, $kuka_tunnus) {
	global $kukarow;

	$query = "	SELECT IFNULL(round(sum(lasku.arvo),0), 0) myynti,
				IFNULL(round(sum(lasku.kate),0), 0) kate
				FROM lasku use index (yhtio_tila_tapvm)
				WHERE lasku.yhtio = '{$kukarow["yhtio"]}'
				and lasku.tila = 'L'
				and lasku.alatila = 'X'
				and lasku.tapvm >= '{$aika['start_date']}'
				and lasku.tapvm <= '{$aika['end_date']}'
				and lasku.myyja = '{$kuka_tunnus}'";
	$result = pupe_query($query);

	return mysql_fetch_assoc($result);
}

function piirra_aikavali($aika_vali) {
	return date('d.m', strtotime($aika_vali['start_date'])).' - '.date('d.m', strtotime($aika_vali['end_date']));
}

function paata_kvartaali() {
	$tama_vuosi = date('Y');
	$kvartaalit = array(
		4	 => array(
			"start_date" => "$tama_vuosi-01-01",
			"end_date"	 => "$tama_vuosi-03-31"
		),
		1	 => array(
			"start_date" => "$tama_vuosi-04-01",
			"end_date"	 => "$tama_vuosi-06-30"
		),
		2	 => array(
			"start_date" => "$tama_vuosi-07-01",
			"end_date"	 => "$tama_vuosi-09-30"
		),
		3	 => array(
			"start_date" => "$tama_vuosi-10-01",
			"end_date"	 => "$tama_vuosi-12-31"
		),
	);

	//tarkisteaan mik� kvartaali on menossa.
	foreach ($kvartaalit as $kvartaali) {
		$now = date('Y-m-d');
		if ($now >= $kvartaali['start_date'] and $now <= $kvartaali['end_date']) {
			$nykyinen_kvartaali = $kvartaali;
			break;
		}
	}

	return $nykyinen_kvartaali;
}
