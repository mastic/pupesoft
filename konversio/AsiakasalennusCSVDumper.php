<?php

require_once('CSVDumper.php');

class AsiakasalennusCSVDumper extends CSVDumper {

	protected $unique_values = array();

	public function __construct($kukarow) {
		parent::__construct($kukarow);

		$konversio_array = array(
			'asiakas'	 => 'ASIAKAS',
			'ryhma'		 => 'TUNNUS',
			'alennus'	 => 'PROS',
		);
		$required_fields = array(
			'asiakas',
			'ryhma',
			'alennus'
		);

		$this->setFilepath("/tmp/asiakasalennus.csv");
		$this->setSeparator(';');
		$this->setKonversioArray($konversio_array);
		$this->setRequiredFields($required_fields);
		$this->setTable('asiakasalennus');
	}

	protected function konvertoi_rivit() {
		$progressbar = new ProgressBar(t('Konvertoidaan rivit'));
		$progressbar->initialize(count($this->rivit));

		foreach ($this->rivit as $index => &$rivi) {
			$rivi = $this->decode_to_utf8($rivi);
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
				if ($konvertoitu_header == 'ryhma') {
					//Oletetaan, ett� solusta l�ytyy vain yksi luku jolloin siihen voidaan viitata 0 indeksill�
					$matches = array();
					preg_match('/\d+/', $rivi[$csv_header], $matches);
					$rivi_temp[$konvertoitu_header] = $matches[0];
				}
				else {
					$rivi_temp[$konvertoitu_header] = $rivi[$csv_header];
				}
			}
		}

		return $rivi_temp;
	}

	protected function validoi_rivi(&$rivi, $index) {
		$valid = true;
		foreach ($rivi as $key => $value) {
			if ($key == 'asiakas') {
				$asiakas_tunnus = $this->hae_asiakas_tunnus($value);
				if ($asiakas_tunnus == 0 and in_array($key, $this->required_fields)) {
					$this->errors[$index][] = t('Asiakasta')." <b>{$value}</b> ".t('ei l�ydy');
					$valid = false;
				}
				else {
					$rivi[$key] = $asiakas_tunnus;
				}
			}
			else {
				if (in_array($key, $this->required_fields) and $value == '') {
					$valid = false;
				}
			}
		}

		return $valid;
	}

	private function hae_asiakas_tunnus($asiakasnro) {
		$query = "	SELECT tunnus
					FROM asiakas
					WHERE yhtio = '{$this->kukarow['yhtio']}'
					AND asiakasnro = '{$asiakasnro}'";
		$result = pupe_query($query);
		$asiakasrow = mysql_fetch_assoc($result);

		if (!empty($asiakasrow)) {
			return $asiakasrow['tunnus'];
		}

		return 0;
	}

}