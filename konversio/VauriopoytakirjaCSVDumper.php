<?php

require_once('CSVDumper.php');
require_once('tilauskasittely/luo_myyntitilausotsikko.inc');

class VauriopoytakirjaCSVDumper extends CSVDumper {

	protected $unique_values = array();

	public function __construct(array &$kukarow) {
		parent::__construct($kukarow, false);

		$konversio_array = array(
			'rekno'			 => 0, //tilivuosi: vvvv
			'takuunumero'	 => 1, //nippunro: char(10)
			'prioriteetti'	 => 2, //kiireellisyysluokka: char(1)
			'valmis'		 => 3, //valmistumispvm (ty� p��ttyi): char(12) vvvvkkpphhmi
			'viite'			 => 4, //teleliikennealue (TLA): char(3)
			'suorittaja'	 => 5, //ty�alue: char(10) selvityksen antaja/urakoitsija
			'jalleenmyyja'	 => 6, //verkostoalue: char(6)
			'poistui'		 => 7, //aloitettupvm: char(12) ty� alkoi
			'vpk_valmis'	 => 8, //valmistumispvm (tekn. valmis, ty� p��ttyi): char(12) vvvvkkpphhmi
			'komm1'			 => 10, //lisatieto: char(80) lis�tietoja soneralla ja tapahtumapaikka
		);
		$required_fields = array(
			'takuunumero',
		);
		$mandatory_fields = array(
			'lisakuluprosentti'	 => 7,
			'tyostatus'			 => 1, //Urakoitsijalla = 1
		);

		$this->setFolder('/tmp/sp/');
		$this->setValmisFolder('/tmp/sp/valmis/');
		$this->setErrorFolder('/tmp/sp/error/');
		$this->setSeparator(';');
		$this->setKonversioArray($konversio_array);
		$this->setRequiredFields($required_fields);
		$this->addMandatoryFields($mandatory_fields);
		$this->setTable('tyomaarays');
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
				if (in_array($konvertoitu_header, array('valmis', 'poistui', 'vpk_valmis'))) {
					$year = substr($rivi[$csv_header], 0, 4);
					$month = substr($rivi[$csv_header], 4, 2);
					$day = substr($rivi[$csv_header], 6, 2);
					$hour = substr($rivi[$csv_header], 8, 2);
					$minutes = substr($rivi[$csv_header], 10, 2);

					$rivi_temp[$konvertoitu_header] = "$year-$month-$day $hour:$minutes:00";
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
			if (in_array($key, $this->required_fields) and $value == '') {
				$this->errors[$index][] = t('Pakollinen kentt�')." <b>{$key}</b> ".t('puuttuu');
				$valid = false;
			}
		}

		return $valid;
	}

	protected function dump_data() {
		$progress_bar = new ProgressBar(t('Ajetaan rivit tietokantaan'));
		$progress_bar->initialize(count($this->rivit));
		$this->kukarow['kesken'] = 0;
		foreach ($this->rivit as $rivi) {
			//@TODO ty�m��r�ykset liitet��n toistaiseksi kaato-asiakkaalle. muuta t�m�. aineistossa tulee vahingon aiheuttaja
			$asiakas = hae_asiakas(100);
			$lasku_tunnus = luo_myyntitilausotsikko('TYOMAARAYS', $asiakas['tunnus']);

			$query = "	UPDATE {$this->table}\nSET ";
			foreach ($rivi as $key => $value) {
				if ($key == 'luontiaika') {
					$query .= "{$key} = {$value},";
				}
				else {
					$query .= "{$key} = '{$value}',";
				}
			}
			$query = substr($query, 0, -1);
			$query .= " WHERE otunnus = '{$lasku_tunnus}'";

			pupe_query($query);

			$this->kukarow['kesken'] = 0;
			$progress_bar->increase();
		}
	}

}