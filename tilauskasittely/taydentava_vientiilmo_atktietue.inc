<?php

	function atk_eratietue () {
		global $atk, $yhtiorow, $vtietue, $laskurow, $atkrivi, $vientiarvo;
		
		$atk .= sprintf ('%-13.13s', 	str_replace('-','',$laskurow["tullausnumero"]));
		$atk .= sprintf ('%04d',		$atkrivi);				//rivinumero
		$atk .= sprintf ('%03d', 		"");					//Tyhj�
		$atk .= sprintf ('%-2.2s', 		"01");					//Tietuetyyppi
		$atk .= sprintf ('%-1.1s', 		"");					//Tyhj�
		$atk .= sprintf ('%04d', 		$vtietue);				//viimeisen tietueen j�rjestysnumero
		$atk .= sprintf ('%-1.1s',		"E");					//K�sittelytyyppi
		$atk .= sprintf ('%-2.2s', 		"11");					//Tullipiirin johto hardcoded
		$atk .= sprintf ('%-6.6s', 		"");					//Tyhj�
		$atk .= sprintf ('%-2.2s', 		$yhtiorow["tullikamari"]);	//Valvova tullipiiri
		$atk .= sprintf ('%-2.2s', 		"");					//Tapcossa tyhj�
		$atk .= sprintf ('%-3.3s', 		substr(str_replace('-','',$laskurow["tullausnumero"]),7,3)); //VXX
		$atk .= sprintf ('%05d',	 	substr(str_replace('-','',$laskurow["tullausnumero"]),0,2).substr(str_replace('-','',$laskurow["tullausnumero"]),4,3)); //VVPPP
		$atk .= sprintf ('%-1.1s', 		"S");					//Kielikoodi
		$atk .= sprintf ('%-1.1s', 		"1");					//Vienti kyseess�
		$atk .= sprintf ('%-1.1s', 		"R");					//Rekister�ity asiakas
		$atk .= sprintf ('%-1.1s', 		"");					//Ei ilmoiteta
		$atk .= sprintf ('%-1.1s',		"Z");					//Kotitullaus
		$atk .= sprintf ('%06d',	 	substr($laskurow["tapvm"],8,2).substr($laskurow["tapvm"],5,2).substr($laskurow["tapvm"],2,2));	//PPKKVV
		$atk .= sprintf ('%-4.4s', 		"");					//Ei ilmoiteta
		$atk .= sprintf ('%05d',	 	str_replace('-','',$yhtiorow["tullin_asiaknro"]));		
		$atk .= sprintf ('%05d', 		"");					//Ei ilmoiteta
		$atk .= sprintf ('%-15.15s', 	$laskurow["laskunro"]);	//Huolinnan viite (lakunro)
		$atk .= sprintf ('%-1.1s', 		$laskurow["kuljetusmuoto"]);
		$atk .= sprintf ('%-1.1s', 		$laskurow["sisamaan_kuljetusmuoto"]);
		$atk .= sprintf ('%-1.1s', 		$laskurow["kontti"]);
		$atk .= sprintf ('%-2.2s', 		$laskurow["aktiivinen_kuljetus_kansallisuus"]);
		$atk .= sprintf ('%-3.3s', 		$laskurow["poistumistoimipaikka_koodi"]);
		$atk .= sprintf ('%06d',	 	substr($laskurow["tapvm"],8,2).substr($laskurow["tapvm"],5,2).substr($laskurow["tapvm"],2,2));	//PPKKVV
		$atk .= sprintf ('%-4.4s', 		"");					//Tyhj�
		$atk .= sprintf ('%05d', 		substr(str_replace('-','',$laskurow["tullausnumero"]),0,2).substr(str_replace('-','',$laskurow["tullausnumero"]),4,3));	//VVPPP
		
		$atk .= sprintf ('%02d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-6.6s', 		"");					//Tyhj�
		$atk .= sprintf ('%-12.12s', 	"");					//Tyhj�
		$atk .= sprintf ('%-8.8s', 		"");					//Tyhj�
		$atk .= sprintf ('%06d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-2.2s', 		"");					//Tyhj�
		$atk .= sprintf ('%010d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-4.4s', 		"");					//Tyhj�
		$atk .= sprintf ('%010d',	 	"");					//Tyhj�
		
		$atk .= sprintf ('%02d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-6.6s', 		"");					//Tyhj�
		$atk .= sprintf ('%-12.12s', 	"");					//Tyhj�
		$atk .= sprintf ('%-8.8s', 		"");					//Tyhj�
		$atk .= sprintf ('%06d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-2.2s', 		"");					//Tyhj�
		$atk .= sprintf ('%010d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-4.4s', 		"");					//Tyhj�
		$atk .= sprintf ('%010d',	 	"");					//Tyhj�
		
		$atk .= sprintf ('%02d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-6.6s', 		"");					//Tyhj�
		$atk .= sprintf ('%-12.12s', 	"");					//Tyhj�
		$atk .= sprintf ('%-8.8s', 		"");					//Tyhj�
		$atk .= sprintf ('%06d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-2.2s', 		"");					//Tyhj�
		$atk .= sprintf ('%010d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-4.4s', 		"");					//Tyhj�
		$atk .= sprintf ('%010d',	 	"");					//Tyhj�
		
		$atk .= sprintf ('%02d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-6.6s', 		"");					//Tyhj�
		$atk .= sprintf ('%-12.12s', 	"");					//Tyhj�
		$atk .= sprintf ('%-8.8s', 		"");					//Tyhj�
		$atk .= sprintf ('%06d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-2.2s', 		"");					//Tyhj�
		$atk .= sprintf ('%010d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-4.4s', 		"");					//Tyhj�
		$atk .= sprintf ('%010d',	 	"");					//Tyhj�
		
		$atk .= sprintf ('%-11.11s', 	str_replace('-','',sprintf ('%08d',$yhtiorow["ytunnus"])));	//Ytunnus
		$atk .= sprintf ('%-1.1s', 		"");					//Tyhj�
		$atk .= sprintf ('%06d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-1.1s',		"");					//Tyhj�
		$atk .= sprintf ('%014d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-7.7s', 		"");					//Tyhj�
		$atk .= sprintf ('%022d',	 	"");					//Tyhj�	
		$atk .= sprintf ('%011d',	 	"");					//Tyhj�
		$atk .= sprintf ('%011d',	 	"");					//Tyhj�
		$atk .= sprintf ('%037d',		"");					//Tyhj�
		$atk .= sprintf ('%-15.15s', 	"");					//Tyhj�
		$atk .= sprintf ('%012d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-2.2s', 		"");					//Tyhj�
		$atk .= sprintf ('%06d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-185.185s',	"");					//Tyhj�
		$atk .= sprintf ('%-2.2s', 		"");					//Tyhj�
		$atk .= sprintf ('%-293.293s',	"");					//Tyhj�
		$atk .= sprintf ('%-3.3s', 		"EUR");					//Valuutta
		$atk .= sprintf ('%-10.10s', 	"");					//Tyhj�

	}
		
	function atk_arvotietue () {
		global $atk, $yhtiorow, $laskurow, $atkrivi, $vientiarvo, $laskunarvo;
		
		$atk .= sprintf ('%-13.13s', 	str_replace('-','',$laskurow["tullausnumero"]));
		$atk .= sprintf ('%04d',		$atkrivi);
		$atk .= sprintf ('%03d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-2.2s', 		"03");					//Tietuetyyppi
		$atk .= sprintf ('%-1.1s', 		"");					//Tyhj�
		$atk .= sprintf ('%-1.1s', 		"2");					//Tapahtumalaji
		$atk .= sprintf ('%-11.11s', 	str_replace('-','',sprintf ('%08d',$yhtiorow["ytunnus"])));	//Ytunnus
		$atk .= sprintf ('%-9.9s',		$laskurow["laskunro"]);	//Lakunro viitteen�
		$atk .= sprintf ('%-17.17s',	"");					//Tyhj�
		$atk .= sprintf ('%-2.2s', 		"");					//Tyhj�
		$atk .= sprintf ('%-1.1s', 		"");					//Tyhj�
		$atk .= sprintf ('%-1.1s', 		"");					//Ei ilmoiteta
		$atk .= sprintf ('%-3.3s', 		"EXW");					//Incoterms, nyt hardcoded!!!!!!!!!!!!!!
		$atk .= sprintf ('%-32.32s', 	"");					//Ei ilmoiteta
		
		$atk .= sprintf ('%-3.3s',	 	$laskurow["valkoodi"]);
		
		if ($laskurow["valkoodi"] != 'EUR') {
			$atk .= sprintf ('%014d', 	round($vientiarvo,0));	//Valuuttam��r�, muut kuin EUR
		}
		else {
			$atk .= sprintf ('%014d', 	"");					//Valuuttam��r�	
		}
		
		$atk .= sprintf ('%-3.3s',		"");					//Tyhj�
		$atk .= sprintf ('%014d', 		"");					//Tyhj�
		
		$atk .= sprintf ('%012d',	 	"");					//Tyhj�
		
		$atk .= sprintf ('%-2.2s', 		"");					//Ei pakollinen
		$atk .= sprintf ('%012d', 		"");					//Tyhj�
		$atk .= sprintf ('%-2.2s', 		"");					//Ei pakollinen
		$atk .= sprintf ('%012d', 		"");					//Tyhj�
		$atk .= sprintf ('%-2.2s', 		"");					//Ei pakollinen
		$atk .= sprintf ('%012d', 		"");					//Tyhj�
		$atk .= sprintf ('%-2.2s', 		"");					//Ei pakollinen
		$atk .= sprintf ('%012d', 		"");					//Tyhj�
		$atk .= sprintf ('%-2.2s', 		"");					//Ei pakollinen
		$atk .= sprintf ('%012d', 		"");					//Tyhj�
		
		$atk .= sprintf ('%-2.2s', 		"");					//Ei pakollinen
		$atk .= sprintf ('%012d', 		"");					//Tyhj�
		$atk .= sprintf ('%-2.2s', 		"");					//Ei pakollinen
		$atk .= sprintf ('%012d', 		"");					//Tyhj�
		
		$atk .= sprintf ('%024d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-147.147s', 	"");					//Tyhj�
		$atk .= sprintf ('%010d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-13.13s', 	"");					//Tyhj�
		$atk .= sprintf ('%08d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-13.13s', 	"");					//Tyhj�
		$atk .= sprintf ('%08d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-13.13s', 	"");					//Tyhj�
		$atk .= sprintf ('%08d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-13.13s', 	"");					//Tyhj�
		$atk .= sprintf ('%08d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-13.13s', 	"");					//Tyhj�
		$atk .= sprintf ('%020d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-12.12s', 	"");					//Tyhj�
		$atk .= sprintf ('%-2.2s', 		$laskurow["kauppatapahtuman_luonne"]);
		$atk .= sprintf ('%012d', 		"");					//Tyhj�			
		$atk .= sprintf ('%012d', 		str_replace('.','',$laskurow["lisattava_era"]));	//lis�tt�v� kustannus
		$atk .= sprintf ('%012d',	 	str_replace('.','',$laskurow["vahennettava_era"]));	//v�hennett�v� kustannus
		$atk .= sprintf ('%-12.12s', 	"");					//Tyhj�
		$atk .= sprintf ('%012d', 		str_replace('.','',$laskunarvo)); //Summa ilman desimaalierotinta
		$atk .= sprintf ('%-384.384s', 	"");					//Tyhj�

	}
	
	function atk_nimiketietue () {
		global $atk, $yhtiorow, $laskurow, $rivirow, $atkrivi, $tullarvo, $vientiarvo;
		
		$atk .= sprintf ('%-13.13s', 	str_replace('-','',$laskurow["tullausnumero"]));
		$atk .= sprintf ('%04d',		$atkrivi);
		$atk .= sprintf ('%03d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-2.2s', 		"04");					//Tietuetyyppi
		$atk .= sprintf ('%-1.1s', 		"");					//Tyhj�
		$atk .= sprintf ('%-8.8s', 		$rivirow["tullinimike1"]);
		$atk .= sprintf ('%-2.2s', 		"");
		$atk .= sprintf ('%-31.31s',	"");					//Tyhj�
		$atk .= sprintf ('%-2.2s',		$laskurow["maa_maara"]);
		$atk .= sprintf ('%-3.3s', 		"");					//Tyhj�
		$atk .= sprintf ('%04d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-2.2s', 		"");					//Tyhj�
		$atk .= sprintf ('%010d',	 	"");					//Tyhj�
		$atk .= sprintf ('%010d',	 	"");					//Tyhj�
		$atk .= sprintf ('%010d',	 	"");					//Tyhj�
		
		$atk .= sprintf ('%010d',	 	$rivirow["nettop"]);	//Paljouden m��r�
		$atk .= sprintf ('%-4.4s',	 	"KGM");					//Paljouden yksikk�
		
		if ($rivirow["su"] != '') {
			$atk .= sprintf ('%010d',	$rivirow["kpl"]);		//2-paljouden m��r�
			$atk .= sprintf ('%-4.4s',	$rivirow["su_vientiilmo"]);		//2-paljous, XXX muodossa
		}
		else {
			$atk .= sprintf ('%010d',	 "");					//Tyhj�
			$atk .= sprintf ('%-4.4s',	 "");					//Tyhj�
		}
		
		$atk .= sprintf ('%010d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-4.4s',	 	"");					//Tyhj�
		
		$atk .= sprintf ('%02d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-6.6s',	 	"");					//Tyhj�
		$atk .= sprintf ('%-12.12s', 	"");					//Tyhj�
		$atk .= sprintf ('%-8.8s',	 	"");					//Tyhj�
		$atk .= sprintf ('%06d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-2.2s',	 	"");					//Tyhj�
		$atk .= sprintf ('%010d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-4.4s',	 	"");					//Tyhj�
		$atk .= sprintf ('%010d',	 	"");					//Tyhj�
		
		$atk .= sprintf ('%02d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-6.6s',	 	"");					//Tyhj�
		$atk .= sprintf ('%-12.12s', 	"");					//Tyhj�
		$atk .= sprintf ('%-8.8s',	 	"");					//Tyhj�
		$atk .= sprintf ('%06d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-2.2s',	 	"");					//Tyhj�
		$atk .= sprintf ('%010d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-4.4s',	 	"");					//Tyhj�
		$atk .= sprintf ('%010d',	 	"");					//Tyhj�
		
		$atk .= sprintf ('%02d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-6.6s',	 	"");					//Tyhj�
		$atk .= sprintf ('%-12.12s', 	"");					//Tyhj�
		$atk .= sprintf ('%-8.8s',	 	"");					//Tyhj�
		$atk .= sprintf ('%06d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-2.2s',	 	"");					//Tyhj�
		$atk .= sprintf ('%010d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-4.4s',	 	"");					//Tyhj�
		$atk .= sprintf ('%010d',	 	"");					//Tyhj�
		
		$atk .= sprintf ('%02d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-6.6s',	 	"");					//Tyhj�
		$atk .= sprintf ('%-12.12s', 	"");					//Tyhj�
		$atk .= sprintf ('%-8.8s',	 	"");					//Tyhj�
		$atk .= sprintf ('%06d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-2.2s',	 	"");					//Tyhj�
		$atk .= sprintf ('%010d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-4.4s',	 	"");					//Tyhj�
		$atk .= sprintf ('%010d',	 	"");					//Tyhj�
		
		$atk .= sprintf ('%02d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-6.6s',	 	"");					//Tyhj�
		$atk .= sprintf ('%-12.12s', 	"");					//Tyhj�
		$atk .= sprintf ('%-8.8s',	 	"");					//Tyhj�
		$atk .= sprintf ('%06d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-2.2s',	 	"");					//Tyhj�
		$atk .= sprintf ('%010d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-4.4s',	 	"");					//Tyhj�
		$atk .= sprintf ('%010d',	 	"");					//Tyhj�
		
		$atk .= sprintf ('%02d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-6.6s',	 	"");					//Tyhj�
		$atk .= sprintf ('%-12.12s', 	"");					//Tyhj�
		$atk .= sprintf ('%-8.8s',	 	"");					//Tyhj�
		$atk .= sprintf ('%06d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-2.2s',	 	"");					//Tyhj�
		$atk .= sprintf ('%010d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-4.4s',	 	"");					//Tyhj�
		$atk .= sprintf ('%010d',	 	"");					//Tyhj�
		
		$atk .= sprintf ('%02d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-6.6s',	 	"");					//Tyhj�
		$atk .= sprintf ('%-12.12s', 	"");					//Tyhj�
		$atk .= sprintf ('%-8.8s',	 	"");					//Tyhj�
		$atk .= sprintf ('%06d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-2.2s',	 	"");					//Tyhj�
		$atk .= sprintf ('%010d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-4.4s',	 	"");					//Tyhj�
		$atk .= sprintf ('%010d',	 	"");					//Tyhj�
		
		$atk .= sprintf ('%02d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-6.6s',	 	"");					//Tyhj�
		$atk .= sprintf ('%-12.12s', 	"");					//Tyhj�
		$atk .= sprintf ('%-8.8s',	 	"");					//Tyhj�
		$atk .= sprintf ('%06d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-2.2s',	 	"");					//Tyhj�
		$atk .= sprintf ('%010d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-4.4s',	 	"");					//Tyhj�
		$atk .= sprintf ('%010d',	 	"");					//Tyhj�
		
		$atk .= sprintf ('%02d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-6.6s',	 	"");					//Tyhj�
		$atk .= sprintf ('%-12.12s', 	"");					//Tyhj�
		$atk .= sprintf ('%-8.8s',	 	"");					//Tyhj�
		$atk .= sprintf ('%06d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-2.2s',	 	"");					//Tyhj�
		$atk .= sprintf ('%010d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-4.4s',	 	"");					//Tyhj�
		$atk .= sprintf ('%010d',	 	"");					//Tyhj�
		
		$atk .= sprintf ('%033d',	 	"");					//Tyhj�
		$atk .= sprintf ('%-3.3s',	 	"");					//Tyhj�
		$atk .= sprintf ('%014d',	 	"");					//Tyhj�
		
		$atk .= sprintf ('%-4.4s', 		"");					//Taric lis�koodi 1
		$atk .= sprintf ('%-4.4s', 		"");					//Taric lis�koodi 2
		$atk .= sprintf ('%-17.17s',	"");					//Tyhj�
		$atk .= sprintf ('%-17.17s', 	"");					//Tyhj�
		$atk .= sprintf ('%-12.12s',	"");					//Tyhj�	
		$atk .= sprintf ('%-4.4s', 		$rivirow["tullikohtelu"]); //Vientimenettely
		$atk .= sprintf ('%-14.14s',	"");					//Tyhj�
		$atk .= sprintf ('%-141.141s',	"");					//Tyhj�
		$atk .= sprintf ('%010d',		str_replace('.','',$tullarvo)); //Tullausarvo
		$atk .= sprintf ('%010d',	 	"");					//Ei k�yt�ss�
		$atk .= sprintf ('%-30.30s',	"");					//Tyhj�

	}	

?>
