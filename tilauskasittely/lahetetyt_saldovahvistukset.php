<?php

require ("../inc/parametrit.inc");
require('myyntires/paperitiliote_saldovahvistus.php');

if ($ajax_request) {
	exit;
}

if (!isset($nayta_pdf)) {
	$nayta_pdf = 0;
}

if ($nayta_pdf != 1) {
	echo "<font class='head'>".t("L�hetetyt saldovahvistukset")."</font><hr>";
}

if (!isset($ppa)) {
	$ppa = date('d');
}
if (!isset($kka)) {
	$kka = date('m', strtotime('now - 1 month'));
}
if (!isset($vva)) {
	$vva = date('Y');
}
if (!isset($ppl)) {
	$ppl = date('d');
}
if (!isset($kkl)) {
	$kkl = date('m');
}
if (!isset($vvl)) {
	$vvl = date('Y');
}

if (checkdate($kka, $ppa, $vva)) {
	$alku_paiva = date('Y-m-d', strtotime("{$vva}-{$kka}-{$ppa}"));
}
else {
	$alku_paiva = date('Y-m-d', strtotime("now - 1 month"));
}

if (checkdate($kkl, $ppl, $vvl)) {
	$loppu_paiva = date('Y-m-d', strtotime("{$vvl}-{$kkl}-{$ppl}"));
}
else {
	$loppu_paiva = date('Y-m-d', strtotime("now"));
}

$request = array(
	'tee'				 => $tee,
	'ryhmittely_tyyppi'	 => $ryhmittely_tyyppi,
	'ryhmittely_arvo'	 => $ryhmittely_arvo,
	'lasku_tunnukset'	 => $lasku_tunnukset,
	'ppa'				 => $ppa,
	'kka'				 => $kka,
	'vva'				 => $vva,
	'ppl'				 => $ppl,
	'kkl'				 => $kkl,
	'vvl'				 => $vvl,
	'alku_paiva'		 => $alku_paiva,
	'loppu_paiva'		 => $loppu_paiva,
	'lahetetyt'			 => true,
);

$request['haku_tyypit'] = array(
	'ytunnus'	 => t('Ytunnus'),
	'asiakasnro' => t('Asiakasnumero'),
	'nimi'		 => t('Nimi'),
);

$request['saldovahvistus_viestit'] = hae_saldovahvistus_viestit();

echo_lahetetyt_saldovahvistukset_kayttoliittyma($request);

if ($request['tee'] == 'hae_lahetetyt_saldovahvistukset') {
	$request['saldovahvistukset'] = hae_myyntilaskuja_joilla_avoin_saldo($request);

	echo_lahetetyt_saldovahvistukset($request);
}
else if ($request['tee'] == 'nayta_saldovahvistus_pdf' or $request['tee'] == 'tulosta_saldovahvistus_pdf') {
	//requestissa tulee tietyn ytunnuksen lasku_tunnuksia. T�ll�in $laskut arrayssa on vain yksi solu
	$laskut = hae_myyntilaskuja_joilla_avoin_saldo($request);

	$laskut['saldovahvistus_viesti'] = search_array_key_for_value_recursive($request['saldovahvistus_viestit'], 'selite', $laskut['saldovahvistus_viesti']);
	$laskut['saldovahvistus_viesti'] = $laskut['saldovahvistus_viesti'][0];
	$laskut['laskun_avoin_paiva'] = $laskut['avoin_saldo_pvm'];
	$pdf_filepath = hae_saldovahvistus_pdf($laskut);

	if ($request['tee'] == 'nayta_saldovahvistus_pdf') {
		echo file_get_contents($pdf_filepath);
	}

	//unset, jotta k�ytt�liittym��n tulisi rajausten mukaiset laskut.
	unset($request['lasku_tunnukset']);

	$request['saldovahvistukset'] = hae_myyntilaskuja_joilla_avoin_saldo($request);
	echo_lahetetyt_saldovahvistukset($request);
}

echo "<br/>";
echo "<br/>";

echo "<font class='head'>".t("L�hetettyjen saldovahvistusten selaaminen")."</font><hr>";

echo_lahetettyjen_saldovahvistusten_selaaminen_kayttoliittyma($request);

if ($request['tee'] == 'hae_lahetetyt_saldovahvistukset_aikavali') {
	$request['saldovahvistukset'] = hae_myyntilaskuja_joilla_avoin_saldo($request);
	$request['saldovahvistukset'] = kasittele_saldovahvistukset($request['saldovahvistukset']);

	echo_lahetetyt_saldovahvistukset_aika_vali($request);
}
?>
<style>
	.saldovahvistukset_per_paiva_not_hidden {
		cursor: pointer;
	}
	.saldovahvistukset_per_paiva_hidden {
		cursor: pointer;
	}

	.saldovahvistusrivi_not_hidden {
	}
	.saldovahvistusrivi_hidden {
		display: none;
	}

	.arrow_img {
		float: right;
	}
</style>
<script>
	$(document).ready(function() {
		bind_saldovahvistus_paiva_tr();
	});

	function bind_saldovahvistus_paiva_tr() {
		$('.saldovahvistus_paiva_tr').click(function() {
			var childress_class = $(this).find('.group_class').val();
			if ($(this).hasClass('saldovahvistukset_per_paiva_hidden')) {
				$('.' + childress_class).addClass('saldovahvistusrivi_not_hidden');
				$('.' + childress_class).removeClass('saldovahvistusrivi_hidden');

				$(this).addClass('saldovahvistukset_per_paiva_not_hidden');
				$(this).removeClass('saldovahvistukset_per_paiva_hidden');

				$(this).find('.arrow_img').attr('src', $('#right_arrow').val());
			}
			else {
				$('.' + childress_class).addClass('saldovahvistusrivi_hidden');
				$('.' + childress_class).removeClass('saldovahvistusrivi_not_hidden');

				$(this).addClass('saldovahvistukset_per_paiva_hidden');
				$(this).removeClass('saldovahvistukset_per_paiva_not_hidden');

				$(this).find('.arrow_img').attr('src', $('#down_arrow').val());
			}
		});
	}
</script>
<?php

echo "<input type='hidden' id='down_arrow' value='{$palvelin2}pics/lullacons/arrow-single-down-green.png' />";
echo "<input type='hidden' id='right_arrow' value='{$palvelin2}pics/lullacons/arrow-single-right-green.png' />";

require('inc/footer.inc');

function echo_lahetetyt_saldovahvistukset($request) {
	global $kukarow, $yhtiorow;

	echo "<table>";
	foreach ($request['saldovahvistukset'] as $saldovahvistus) {
		echo_lahetetty_saldovahvistus_rivi($saldovahvistus, $request);
	}

	echo "</table>";
}

function echo_lahetetty_saldovahvistus_rivi($saldovahvistusrivi, $request, $hidden = false) {
	global $kukarow, $yhtiorow;

	$tr_class = "saldovahvistusrivi_not_hidden";
	if ($hidden) {
		$group_class = "laskut_".str_replace('-', '', $saldovahvistusrivi['avoin_saldo_pvm']);
		$tr_class = "saldovahvistusrivi_hidden {$group_class}";
	}
	echo "<tr class='{$tr_class}'>";

	echo "<td valign='top'>";
	$i = 0;
	$asiakasnumerot_string = "";
	foreach ($saldovahvistusrivi['asiakasnumerot'] as $asiakasnumero) {
		$asiakasnumerot_string .= $asiakasnumero.' / ';
		if ($i != 0 and $i % 10 == 0) {
			$asiakasnumerot_string = substr($asiakasnumerot_string, 0, -3);
			$asiakasnumerot_string .= '<br/>';
		}
		$i++;
	}
	$asiakasnumerot_string = substr($asiakasnumerot_string, 0, -3);
	echo $asiakasnumerot_string;
	echo "</td>";

	echo "<td valign='top'>";
	echo $saldovahvistusrivi['asiakas_nimi'];
	echo "</td>";

	echo "<td valign='top'>";
	echo $saldovahvistusrivi['avoin_saldo_summa'];
	echo "</td>";

	echo "<td valign='top'>";
	echo "<form method='POST' action=''>";
	echo "<input type='submit' value='".t('N�yt� pdf')."' />";
	echo "<input type='hidden' name='tee' value='nayta_saldovahvistus_pdf' />";
	echo "<input type='hidden' name='nayta_pdf' value='1' />";
	echo "<input type='hidden' name='saldovahvistus_viesti' value='{$request['saldovahvistus_viesti']}' />";
	echo "<input type='hidden' name='ryhmittely_tyyppi' value='{$request['ryhmittely_tyyppi']}' />";
	echo "<input type='hidden' name='ryhmittely_arvo' value='{$request['ryhmittely_arvo']}' />";
	foreach ($saldovahvistusrivi['lasku_tunnukset'] as $lasku_tunnus) {
		echo "<input type='hidden' class='lasku_tunnus' name='lasku_tunnukset[]' value='{$lasku_tunnus}' />";
	}
	echo "</form>";

	echo "<br/>";

	echo "<form method='POST' action=''>";
	echo "<input type='submit' value='".t('Tulosta pdf')."' />";
	echo "<input type='hidden' name='tee' value='tulosta_saldovahvistus_pdf' />";
	echo "<input type='hidden' name='saldovahvistus_viesti' value='{$request['saldovahvistus_viesti']}' />";
	echo "<input type='hidden' name='ryhmittely_tyyppi' value='{$request['ryhmittely_tyyppi']}' />";
	echo "<input type='hidden' name='ryhmittely_arvo' value='{$request['ryhmittely_arvo']}' />";
	foreach ($saldovahvistusrivi['lasku_tunnukset'] as $lasku_tunnus) {
		echo "<input type='hidden' name='lasku_tunnukset[]' value='{$lasku_tunnus}' />";
	}
	echo "</form>";
	echo "</td>";

	echo "</tr>";
}

function echo_lahetetyt_saldovahvistukset_kayttoliittyma($request) {
	global $kukarow, $yhtiorow;

	echo "<table>";

	echo "<tr>";
	echo "<th>".t('Haku').":</th>";
	echo "<td>";

	echo "<form method='POST' action=''>";

	echo "<input type='hidden' name='tee' value='hae_lahetetyt_saldovahvistukset' />";

	echo "<select name='ryhmittely_tyyppi'>";
	$sel = "";
	foreach ($request['haku_tyypit'] as $ryhmittely_tyyppi_key => $ryhmittely_tyyppi) {
		if ($request['ryhmittely_tyyppi'] == $ryhmittely_tyyppi) {
			$sel = "SELECTED";
		}
		echo "<option value='{$ryhmittely_tyyppi_key}' {$sel}>{$ryhmittely_tyyppi}</option>";
		$sel = "";
	}
	echo "</select>";

	echo "<input type='text' name='ryhmittely_arvo' value='{$request['ryhmittely_arvo']}' />";

	echo "<input type='submit' value='".t('Etsi')."' />";

	echo "</form>";

	echo "</td>";
	echo "</tr>";

	echo "</table>";
}

function echo_lahetettyjen_saldovahvistusten_selaaminen_kayttoliittyma($request) {
	global $kukarow, $yhtiorow;

	echo "<form method='POST' action=''>";

	echo "<input type='hidden' name='tee' value='hae_lahetetyt_saldovahvistukset_aikavali' />";

	echo "<table>";

	echo "<tr>";
	echo "<th>".t('Alkup�iv�m��r�').":</th>";
	echo "<td>";
	echo "<input type='text' name='ppa' value='{$request['ppa']}' size='3' />";
	echo "<input type='text' name='kka' value='{$request['kka']}' size='3' />";
	echo "<input type='text' name='vva' value='{$request['vva']}' size='5' />";
	echo "</td>";
	echo "</tr>";

	echo "<tr>";
	echo "<th>".t('Loppup�iv�m��r�').":</th>";
	echo "<td>";
	echo "<input type='text' name='ppl' value='{$request['ppl']}' size='3' />";
	echo "<input type='text' name='kkl' value='{$request['kkl']}' size='3' />";
	echo "<input type='text' name='vvl' value='{$request['vvl']}' size='5' />";
	echo "</td>";
	echo "</tr>";

	echo "</table>";

	echo "<input type='submit' value='".t('Etsi')."' />";

	echo "</form>";
}

function kasittele_saldovahvistukset($saldovahvistukset) {
	global $kukarow, $yhtiorow;

	$saldovahvistukset_temp = array();

	foreach ($saldovahvistukset as $saldovahvistus) {
		$saldovahvistukset_temp[$saldovahvistus['lahetys_paiva']][] = $saldovahvistus;
	}

	return $saldovahvistukset_temp;
}

function echo_lahetetyt_saldovahvistukset_aika_vali($request) {
	global $kukarow, $yhtiorow, $palvelin2;

	echo "<table>";

	echo "<tr>";
	echo "<th>".t('L�hetetyt saldovahvistukset')."</th>";
	echo "</tr>";

	foreach ($request['saldovahvistukset'] as $paiva => $saldovahvistukset_per_paiva) {
		echo "<tr class='saldovahvistus_paiva_tr saldovahvistukset_per_paiva_hidden'>";

		echo "<td>";
		echo date('d.m.Y', strtotime($paiva));
		echo "<input type='hidden' class='group_class' value='laskut_".str_replace('-', '', $paiva)."' />";
		echo "<img class='arrow_img' src='{$palvelin2}pics/lullacons/arrow-single-down-green.png' />";
		echo "</td>";

		echo "<td>";
		echo count($saldovahvistukset_per_paiva);
		echo "</td>";

		foreach ($saldovahvistukset_per_paiva as $saldovahvistusrivi) {
			echo_lahetetty_saldovahvistus_rivi($saldovahvistusrivi, $request, true);
		}

		echo "</tr>";
	}

	echo "</table>";
}