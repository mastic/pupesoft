<?php

require_once('CSVDumper.php');

class LaiteCSVDumper extends CSVDumper {

	protected $unique_values = array();

	public function __construct($kukarow) {
		parent::__construct($kukarow);

		$konversio_array = array(
			'tuoteno'		 => 'MALLI',
			'nimitys'		 => 'NIMI',
			'tyyppi'		 => 'TYYPPI',
			'koko'			 => 'PAINO',
			'sarjanro'		 => 'MITAT',
			'valm_pvm'		 => 'DATA10',
			'oma_numero'	 => 'DATA20',
			'paikka'		 => 'LISASIJAINTI',
			'sijainti'		 => 'LISASIJAINTI',
			'koodi'			 => 'KOODI',
			'kohde'			 => 'SIJAINTI', //jos paikalla ei ole nime�, muodostetaan nimi SIJAINTI - TASO3, n�m� pit�� unsettaa ennen dumppia
			'paikka_tark'	 => 'TASO3', //jos paikalla ei ole nime�, muodostetaan nimi SIJAINTI - TASO3, n�m� pit�� unsettaa ennen dumppia
			'asiakas_nimi'	 => 'KUSTPAIKKA', //koska paikan ja kohteen nimien yhdistelm� ei ole uniikki pit�� aineistosta lukea my�s asiakkaan nimi, jonka avulla laite saadaa lis�tty� paikkaan. n�m� pit�� unsettaa ennen dumppia
		);
		$required_fields = array(
			'tuoteno',
			'paikka',
		);

		$this->setFilepath("/tmp/konversio/LAITE.csv");
		$this->setSeparator(';#x#');
		$this->setKonversioArray($konversio_array);
		$this->setRequiredFields($required_fields);
		$this->setTable('laite');
	}

	protected function konvertoi_rivit() {
		$progressbar = new ProgressBar(t('Konvertoidaan rivit'));
		$progressbar->initialize(count($this->rivit));

		foreach ($this->rivit as $index => &$rivi) {
			$rivi = $this->konvertoi_rivi($rivi);
			$rivi = $this->lisaa_pakolliset_kentat($rivi);

			//index + 2, koska eka rivi on header ja laskenta alkaa rivilt� 0
			$valid = $this->validoi_rivi($rivi, $index + 2);

			if (!$valid) {
				unset($this->rivit[$index]);
			}

			$progressbar->increase();
		}
	}

	protected function konvertoi_rivi($rivi) {
		$rivi_temp = array();

		foreach ($this->konversio_array as $konvertoitu_header => $csv_header) {
			if (array_key_exists($csv_header, $rivi)) {
				if ($konvertoitu_header == 'tyyppi') {
					$rivi_temp[$konvertoitu_header] = $rivi[$csv_header].'sammutin';
				}
				else if ($konvertoitu_header == 'paino') {
					$rivi_temp[$konvertoitu_header] = $rivi[$csv_header];
				}
				else if ($konvertoitu_header == 'tuoteno') {
					$rivi_temp[$konvertoitu_header] = str_replace(' ', '', strtoupper($rivi[$csv_header]));
				}
				else {
					$rivi_temp[$konvertoitu_header] = $rivi[$csv_header];
				}
			}
		}

		$rivi_temp['tila'] = 'N';

		return $rivi_temp;
	}

	protected function validoi_rivi(&$rivi, $index) {
		$valid = true;
		foreach ($rivi as $key => $value) {
			if ($key == 'paikka') {
				if ($value != '') {
					$paikka_tunnus = $this->hae_paikka_tunnus($value, $rivi['kohde'], $rivi['asiakas_nimi']);
					$paikan_nimi = $value;
				}
				else {
					$paikka_tunnus = $this->hae_paikka_tunnus($rivi['kohde'].' - '.$rivi['paikka_tark'], $rivi['kohde'], $rivi['asiakas_nimi']);
					$paikan_nimi = $rivi['kohde'].' - '.$rivi['paikka_tark'];
				}
				if ($paikka_tunnus == 0 and in_array($key, $this->required_fields)) {
					$this->errors[$index][] = t('Paikkaa')." <b>{$paikan_nimi}</b> ".t('ei l�ydy');
					$valid = false;
				}
				else {
					$rivi[$key] = $paikka_tunnus;
				}
			}
			else if ($key == 'tuoteno') {
				if ($valid) {
					$valid = $this->loytyyko_tuote($rivi[$key]);
					if (!$valid) {
						list($valid, $tuoteno) = $this->loytyyko_tuote_nimella($rivi['nimitys']);
						if (!$valid) {
							$this->luo_tuote($rivi['tuoteno'], $rivi['tyyppi'], $rivi['koko']);
							$rivi[$key] = $rivi['tuoteno'];
							$valid = true;
						}
						else {
							$rivi[$key] = $tuoteno;
						}
					}
				}
			}
			else {
				if (in_array($key, $this->required_fields) and $value == '') {
					$valid = false;
				}
			}
		}

		unset($rivi['tyyppi']);
		unset($rivi['koko']);
		unset($rivi['nimitys']);
		unset($rivi['kohde']);
		unset($rivi['paikka_tark']);
		unset($rivi['asiakas_nimi']);

		return $valid;
	}

	private function loytyyko_tuote($tuoteno) {
		$query = "	SELECT tunnus
					FROM tuote
					WHERE yhtio = '{$this->kukarow['yhtio']}'
					AND tuoteno = '{$tuoteno}'";
		$result = pupe_query($query);
		if (mysql_num_rows($result) == 1) {
			return true;
		}

		return false;
	}

	private function loytyyko_tuote_nimella($nimitys) {
		$query = "	SELECT tuoteno "
				."FROM tuote "
				."WHERE yhtio = '{$this->kukarow['yhtio']}' "
				."AND nimitys = '{$nimitys}'";
		$result = pupe_query($query);

		if (mysql_num_rows($result) == 1) {
			$tuote = mysql_fetch_assoc($result);
			return array(true, $tuote['tuoteno']);
		}

		return array(false, '');
	}

	private function luo_tuote($tuoteno, $tyyppi, $koko) {
		$query = "	INSERT INTO tuote "
				."SET yhtio = '{$this->kukarow['yhtio']}', "
				."tuoteno = '{$tuoteno}', "
				."nimitys = '{$tuoteno}', "
				."try = '80', "
				."tuotetyyppi = '', "
				."ei_saldoa = '',"
				."laatija = 'import',"
				."luontiaika = NOW()";
		pupe_query($query);

		$query = '	INSERT INTO tuotteen_avainsanat
						(
							yhtio,
							tuoteno,
							kieli,
							laji,
							selite,
							laatija,
							luontiaika
						)
						VALUES
						(
							"'.$this->kukarow['yhtio'].'",
							"'.$tuoteno.'",
							"fi",
							"sammutin_tyyppi",
							"'.$tyyppi.'",
							"import",
							NOW()
						)';
		pupe_query($query);

		$query = '	INSERT INTO tuotteen_avainsanat
						(
							yhtio,
							tuoteno,
							kieli,
							laji,
							selite,
							laatija,
							luontiaika
						)
						VALUES
						(
							"'.$this->kukarow['yhtio'].'",
							"'.$tuoteno.'",
							"fi",
							"sammutin_koko",
							"'.$koko.'",
							"import",
							NOW()
						)';
		pupe_query($query);
	}

	private function hae_paikka_tunnus($paikan_nimi, $kohde_nimi, $asiakas_nimi) {
		$asiakas_join = '	JOIN asiakas
							ON ( asiakas.yhtio = kohde.yhtio
								AND asiakas.tunnus = kohde.asiakas
								AND asiakas.nimi = "'.$asiakas_nimi.'" )';
		$query = '	SELECT paikka.tunnus
					FROM paikka
					JOIN kohde
					ON ( kohde.yhtio = paikka.yhtio
						AND kohde.tunnus = paikka.kohde
						AND kohde.nimi = "'.$kohde_nimi.'" )
					WHERE paikka.yhtio = "'.$this->kukarow['yhtio'].'"
					AND paikka.nimi = "'.$paikan_nimi.'"';
		$result = pupe_query($query);

		if (mysql_num_rows($result) == 0) {
			return 0;
		}

		//jos paikan ja kohteen nimen yhdistelm� ei ole uniikki niin, kokeillaan lis�t� asiakkaan nimi
		if (mysql_num_rows($result) != 1) {
			$query = '	SELECT paikka.tunnus
						FROM paikka
						JOIN kohde
						ON ( kohde.yhtio = paikka.yhtio
							AND kohde.tunnus = paikka.kohde
							AND kohde.nimi = "'.$kohde_nimi.'" )
						'.$asiakas_join.'
						WHERE paikka.yhtio = "'.$this->kukarow['yhtio'].'"
						AND paikka.nimi = "'.$paikan_nimi.'"';
			$result = pupe_query($query);
		}

		$paikkarow = mysql_fetch_assoc($result);

		if (!empty($paikkarow)) {
			return $paikkarow['tunnus'];
		}

		return 0;
	}

	protected function tarkistukset() {
		$query = "	SELECT paikka.tunnus
					FROM paikka
					WHERE yhtio = '{$this->kukarow['yhtio']}'";
		$result = pupe_query($query);

		$paikat = array();
		while($paikka = mysql_fetch_assoc($result)) {
			$paikat[] = $paikka;
		}

		$query = "	SELECT DISTINCT laite.paikka
					FROM laite
					WHERE yhtio = '{$this->kukarow['yhtio']}'";
  		$result = pupe_query($query);
		$laitteiden_paikat = array();
		while($laitteen_paikka = mysql_fetch_assoc($result)) {
			$laitteiden_paikat[] = $laitteen_paikka;
		}

		$kpl = count($paikat) - count($laitteiden_paikat);;

		echo "{$kpl} paikkaa ilman laitetta!!!!";
	}

}