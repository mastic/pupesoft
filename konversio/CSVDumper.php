<?php

require_once('inc/ProgressBar.class.php');

abstract class CSVDumper {

	protected $filepath = "";
	protected $separator = "";
	protected $konversio_array = array();
	protected $required_fields = array();
	protected $columns_to_be_utf8_decoded = array();
	protected $table = "";
	protected $rivit = array();
	protected $kukarow = array();
	protected $errors = array();
	private $mandatory_fields = array();

	public function __construct($kukarow) {
		$this->kukarow = $kukarow;

		$this->mandatory_fields = array(
			'yhtio'		 => $this->kukarow['yhtio'],
			'laatija'	 => 'import',
			'luontiaika' => 'now()',
		);
	}

	protected function setKonversioArray($konversio_array) {
		$this->konversio_array = $konversio_array;
	}

	protected function setRequiredFields($required_fields) {
		$this->required_fields = $required_fields;
	}

	protected function setColumnsToBeUtf8Decoded($columns_to_be_utf8_decoded) {
		$this->columns_to_be_utf8_decoded = $columns_to_be_utf8_decoded;
	}

	protected function setTable($table) {
		$this->table = $table;
	}

	protected function setKukaRow($kukarow) {
		$this->kukarow = $kukarow;
	}

	protected function setMandatoryFields($mandatory_fields) {
		$this->mandatory_fields = $mandatory_fields;
	}

	protected function setFilepath($filepath) {
		$this->filepath = $filepath;
	}

	protected function setSeparator($separator) {
		$this->separator = $separator;
	}

	protected function getErrors() {
		return $this->errors;
	}

	public function aja() {
		if ($this->filepath == '') {
			throw new Exception('Filepath on tyhj�');
		}

		if ($this->separator == '') {
			throw new Exception('Separator on tyhj�');
		}

		if ($this->table == '') {
			throw new Exception('Table on tyhj�');
		}

		if ($this->konversio_array == '') {
			throw new Exception('Konversio_array on tyhj�');
		}

		$this->lue_csv_tiedosto();

		$this->konvertoi_rivit();

		echo "<br/>";
		
		if (empty($this->errors)) {
			echo t('Kaikki ok ajetaan data kantaan');
			echo "<br/>";
		}
		else {
			foreach ($this->errors as $rivinumero => $row_errors) {
				echo t('Rivill�')." {$rivinumero} ".t('oli seuraavat virheet').":";
				echo "<br/>";
				foreach ($row_errors as $row_error) {
					echo $row_error;
					echo "<br/>";
				}
				echo "<br/>";
			}
		}

		$this->dump_data();
	}

	protected function lue_csv_tiedosto() {
		$csv_headerit = $this->lue_csv_tiedoston_otsikot();
		$file = fopen($this->filepath, "r") or die("Ei aukea!\n");

		$rivit = array();
		$i = 1;
		while ($rivi = fgets($file)) {
			if ($i == 1) {
				$i++;
				continue;
			}

			$rivi = explode($this->separator, $rivi);
			$rivi = $this->to_assoc($rivi, $csv_headerit);

			$rivit[] = $rivi;

			$i++;
		}

		fclose($file);

		$this->rivit = $rivit;
	}

	private function to_assoc($rivi, $csv_headerit) {
		$rivi_temp = array();
		foreach ($rivi as $index => $value) {
			$rivi_temp[strtoupper($csv_headerit[$index])] = $value;
		}

		return $rivi_temp;
	}

	private function lue_csv_tiedoston_otsikot() {
		$file = fopen($this->filepath, "r") or die("Ei aukea!\n");
		$header_rivi = fgets($file);
		if ($this->onko_tiedosto_utf8_bom($header_rivi)) {
			$header_rivi = substr($header_rivi, 3);
		}
		$header_rivi = explode($this->separator, $header_rivi);
		fclose($file);

		return $header_rivi;
	}

	private function onko_tiedosto_utf8_bom($str) {
		$bom = pack("CCC", 0xef, 0xbb, 0xbf);
		if (0 == strncmp($str, $bom, 3)) {
			return true;
		}
		return false;
	}

	protected function dump_data() {
		$progress_bar = new ProgressBar(t('Ajetaan rivit tietokantaan'));
		$progress_bar->initialize(count($this->rivit));
		foreach ($this->rivit as $rivi) {
			$query = '	INSERT INTO '.$this->table.'
					('.implode(", ", array_keys($rivi)).')
					VALUES
					("'.implode('", "', array_values($rivi)).'")';

			pupe_query($query);
			$progress_bar->increase();
		}
	}

	protected function decode_to_utf8($rivi) {
		//@TODO Decode_to_utf8 pit�� koodata niin, ett� columns_to_be_utf8_decoded on csv:lt� l�ytyv�t headerit. T�ll�in Decode_to_utf8 funkkaria voidaan kutsua ennen konvertoi_rivi funkkaria
		//Toistaiseksi paikan konvertoi_rivi kutsuu manuaalisesti utf8_decodea, ett� kustannuspaikka haku toimii (rivi 59)
		foreach ($rivi as $header => &$value) {
			if (in_array($header, $this->columns_to_be_utf8_decoded)) {
				$value = utf8_decode($value);
			}
		}

		return $rivi;
	}

	protected function lisaa_pakolliset_kentat($rivi) {
		foreach ($this->mandatory_fields as $header => $pakollinen_kentta) {
			$rivi[$header] = $pakollinen_kentta;
		}

		return $rivi;
	}

	abstract protected function konvertoi_rivit();

	abstract protected function konvertoi_rivi($rivi, $index);

	abstract protected function validoi_rivi($rivi, $index);
}