<?php

/**
 * Ty�m��r�yksen funktioita
 */
if (!function_exists('tyomaarays_toimintavarmuus')) {

	/**
	 * Toimintavarmuuden laskenta
	 *
	 * @param  string 	$kiireellisyys	Kiireellisyys
	 * @param  int		$paivat			H�iri�n poistumiseen kuluneet p�iv�t
	 * @return float
	 */
	function tyomaarays_toimintavarmuus($kiireellisyys, $paivat) {
		$toimvar = '-';

		switch ($kiireellisyys) {
			case 'A1':
				if ($paivat <= 1.5) {
					$toimvar = 1;
				}
				else {
					$toimvar = 1.5 / $paivat;
				}
				break;
			case 'A2':
				if ($paivat <= 3) {
					$toimvar = 1;
				}
				else {
					$toimvar = 3 / $paivat;
				}
				break;
			case 'A3':
				if ($paivat <= 5) {
					$toimvar = 1;
				}
				else {
					$toimvar = 5 / $paivat;
				}
				break;
			case 'A4':
				if ($paivat <= 7) {
					$toimvar = 1;
				}
				else {
					$toimvar = 7 / $paivat;
				}
				break;
			default:
				break;
		}

		// Palautetaan toimintavarmuusprosentti
		return $toimvar;
	}

}

if (!function_exists('tyomaarays_palautusvarmuus')) {

	/**
	 * Palautusvarmuus laskenta
	 * Jos palautus on alle 7 p�iv�ss� niin palautusvarmuus on 100%
	 * muuten prosentti lasketaan kaavalla 7 / palautus-p�iv�t.
	 *
	 * @param int		$palautus_pvm		P�ivien lukum��r� (vpk_valmis-h�iri�_p��ttyi)
	 * @return float
	 */
	function tyomaarays_palautusvarmuus($palautus_pvm) {
		$palautusvarmuus = '-';

		if ($palautus_pvm <= 7) {
			$palautusvarmuus = 1;
		}
		else {
			$palautusvarmuus = 7 / $palautus_pvm;
		}

		return $palautusvarmuus;
	}

}