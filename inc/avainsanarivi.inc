<?php

	$ulos   = "";
	$jatko  = 1; // oletetaan normaali k�sittely
	$tyyppi = 1; // oletetaan rivin n�kyvyys

	if ($i == 0) { //yhti�t� ei n�ytet�
		$tyyppi = 0;
	}

	// N�it� kentti� ei oletuksena n�ytet�
	if (mysql_field_name($result, $i) == "selitetark_5" or
		mysql_field_name($result, $i) == "selitetark_4" or
		mysql_field_name($result, $i) == "selitetark_3" or
		mysql_field_name($result, $i) == "selitetark_2") {
		$tyyppi = 0;
	}

	if (mysql_field_name($result, $i) == "selitetark_5" and
		$avain_sel['MYSQLALIAS'] == "SELECTED" or
		$avain_sel["SAHKTILTUN"] == "SELECTED") {
		$tyyppi = 1;
	}

	if (mysql_field_name($result, $i) == "selitetark_4" and
		($avain_sel['ASIAKASRYHMA'] == "SELECTED" or
		$avain_sel['HENKILO_OSASTO'] == "SELECTED" or
		$avain_sel['MYSQLALIAS'] == "SELECTED" or
		$avain_sel['TRY'] == "SELECTED" or
		$avain_sel["SAHKTILTUN"] == "SELECTED" or
		$avain_sel['TYOM_TYOKENTAT'] == "SELECTED")) {
		$tyyppi = 1;
	}

	if (mysql_field_name($result, $i) == "selitetark_3" and
		($avain_sel['ASAVAINSANA'] == "SELECTED" or
		$avain_sel['ASIAKASLUOKKA'] == "SELECTED" or
		$avain_sel['HENKILO_OSASTO'] == "SELECTED" or
		$avain_sel['INVEN_LAJI'] == "SELECTED" or
		$avain_sel['MYSQLALIAS'] == "SELECTED" or
		$avain_sel['OSASTO'] == "SELECTED" or
		$avain_sel['PAIKALLISVARAST'] == "SELECTED" or
		$avain_sel['RAHTIKIRJA'] == "SELECTED" or
		$avain_sel['SARJANUMERON_LI'] == "SELECTED" or
		$avain_sel['SQLDBQUERY'] == "SELECTED" or
		$avain_sel['TOIMEHTO'] == "SELECTED" or
		$avain_sel['TRY'] == "SELECTED" or
		$avain_sel['TYOM_PRIORIT'] == "SELECTED" or
		$avain_sel['TYOM_TYOJONO'] == "SELECTED" or
		$avain_sel['TYOM_TYOKENTAT'] == "SELECTED" or
		$avain_sel['TYOM_TYOSTATUS'] == "SELECTED" or
		$avain_sel["SAHKTILTUN"] == "SELECTED" or
		$avain_sel['Y'] == "SELECTED")) {
		$tyyppi = 1;
	}

	if (mysql_field_name($result, $i) == "selitetark_2" and
		($avain_sel['ALV'] == "SELECTED" or
		$avain_sel['ALVULK'] == "SELECTED" or
		$avain_sel['ASAVAINSANA'] == "SELECTED" or
		$avain_sel['ASIAKASLUOKKA'] == "SELECTED" or
		$avain_sel['ASIAKASOSASTO'] == "SELECTED" or
		$avain_sel['ASIAKASRYHMA'] == "SELECTED" or
		$avain_sel['HENKILO_OSASTO'] == "SELECTED" or
		$avain_sel['INVEN_LAJI'] == "SELECTED" or
		$avain_sel['KAANTALVVIESTI'] == "SELECTED" or
		$avain_sel['KALETAPA'] == "SELECTED" or
		$avain_sel['KASSA'] == "SELECTED" or
		$avain_sel['KUKAASEMA'] == "SELECTED" or
		$avain_sel['LAHETETYYPPI'] == "SELECTED" or
		$avain_sel['LASKUTUS_SAATE'] == "SELECTED" or
		$avain_sel['LITETY'] == "SELECTED" or
		$avain_sel['MERCASVHKER'] == "SELECTED" or
		$avain_sel['MYSQLALIAS'] == "SELECTED" or
		$avain_sel['OLETUSPAIKKA'] == "SELECTED" or
		$avain_sel['OSASTO'] == "SELECTED" or
		$avain_sel['OSTOTILAUS_SAAT'] == "SELECTED" or
		$avain_sel['PAIKALLISVARAST'] == "SELECTED" or
		$avain_sel['PIIRI'] == "SELECTED" or
		$avain_sel['RAHTIKIRJA'] == "SELECTED" or
		$avain_sel['SARJANUMERON_LI'] == "SELECTED" or
		$avain_sel['SQLDBQUERY'] == "SELECTED" or
		$avain_sel['TOIMEHTO'] == "SELECTED" or
		$avain_sel['TOIMTAPAKV'] == "SELECTED" or
		$avain_sel['TRY'] == "SELECTED" or
		$avain_sel['TYOM_PRIORIT'] == "SELECTED" or
		$avain_sel['TYOM_TYOJONO'] == "SELECTED" or
		$avain_sel['TYOM_TYOKENTAT'] == "SELECTED" or
		$avain_sel['TYOM_TYOLINJA'] == "SELECTED" or
		$avain_sel['TYOM_TYOSTATUS'] == "SELECTED" or
		$avain_sel['VALMISTUSLINJA'] == "SELECTED" or
		$avain_sel["SAHKTILTUN"] == "SELECTED" or
		$avain_sel["OSTOTIL_TILTYYP"] == "SELECTED" or
		$avain_sel['Y'] == "SELECTED")) {
		$tyyppi = 1;
	}

	if (mysql_field_name($result, $i) == "perhe") {

		// Perhetunnus
		if ($lukitse_avaimeen != "") {
			$trow[$i] = $perhe = $lukitse_avaimeen;
		}
		elseif ($trow[$i] == 0) {
			$query = "	SELECT max(perhe) perhe
			 			FROM avainsana
						WHERE yhtio = '$kukarow[yhtio]'";
			$pres = pupe_query($query);
			$perse = mysql_fetch_array($pres);

			$trow[$i] = $perhe = ($perse["perhe"]+1);

			//P�ivitet��n t�m� jo t�ss� vaiheessa
			if ($tunnus > 0) {
				$query = "	UPDATE avainsana
							SET perhe = '$perhe'
							WHERE yhtio = '$kukarow[yhtio]'
							and tunnus = '$tunnus'";
				$pres = pupe_query($query);
			}
		}
		else {
			$perhe = $trow[$i];
		}

		$tyyppi = 4;
	}

	// Maksuehtojen js toimitustapojen kieliveriot
	if (mysql_field_name($result, $i) == "selite" and $lukitse_avaimeen != "" and ($lukitse_laji == "TOIMTAPAKV" or $lukitse_laji == "MAKSUEHTOKV" or $lukitse_laji == "PAKKAUSKV")) {
		$ulos = "<td><input type='hidden' name='$nimi' value='$lukitse_avaimeen'>$lukitse_avaimeen</td>";
		$jatko = 0;
	}

	if (mysql_field_name($result, $i) == "selite") {
		$avain_selite 	= "SELECTED";
	}

	if (mysql_field_name($result, $i) == "laji") {

		$al_laji 			  = trim(strtoupper($trow[$i]));
		$lukitse_laji 		  = trim(strtoupper($lukitse_laji));
		$avain_sel[$trow[$i]] = "SELECTED";
		$mysqlaliasarraysetti = $trow[$i];

		$ulos = "<td><select name='$nimi' onchange='submit();'>";

		if ($lukitse_laji== "") $ulos .= "<optgroup label='".t("Tuotteiden avainsanat")."'>";
		if ($lukitse_laji== "" or $lukitse_laji == "Y") 				$ulos .= "<option value='Y' $avain_sel[Y]>									".t("Yksikko")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "TRY") 				$ulos .= "<option value='TRY' $avain_sel[TRY]>								".t("Tuoteryhm�")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "OSASTO") 			$ulos .= "<option value='OSASTO' $avain_sel[OSASTO]>						".t("Tuoteosasto")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "TUOTEMERKKI") 		$ulos .= "<option value='TUOTEMERKKI' $avain_sel[TUOTEMERKKI]>				".t("Tuotemerkki")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "MALLI") 			$ulos .= "<option value='MALLI' $avain_sel[MALLI]>							".t("Tuotemalli")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "MALLITARKENNE") 	$ulos .= "<option value='MALLITARKENNE' $avain_sel[MALLITARKENNE]>			".t("Tuotemallitarkenne")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "S") 				$ulos .= "<option value='S' $avain_sel[S]>									".t("Tuotteen status")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "TUOTEULK") 			$ulos .= "<option value='TUOTEULK' $avain_sel[TUOTEULK]>					".t("Tuotteiden avainsanojen laji")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "VARASTOLUOKKA") 	$ulos .= "<option value='VARASTOLUOKKA' $avain_sel[VARASTOLUOKKA]>			".t("Varastoluokka")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "VARASTORYHMA") 		$ulos .= "<option value='VARASTORYHMA' $avain_sel[VARASTORYHMA]>			".t("Varastoryhm�")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "SARJANUMERON_LI") 	$ulos .= "<option value='SARJANUMERON_LI' $avain_sel[SARJANUMERON_LI]>		".t("Sarjanumeron lis�tieto")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "PARAMETRI") 		$ulos .= "<option value='PARAMETRI' $avain_sel[PARAMETRI]>					".t("Tuotteen parametri")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "LISATIETO") 		$ulos .= "<option value='LISATIETO' $avain_sel[LISATIETO]>					".t("Tuotteen lis�tieto")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "TARRATYYPPI") 		$ulos .= "<option value='TARRATYYPPI' $avain_sel[TARRATYYPPI]>				".t("Tuotteen tarratyyppi")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "VALMISTUSLINJA") 	$ulos .= "<option value='VALMISTUSLINJA' $avain_sel[VALMISTUSLINJA]>		".t("Tuotteen valmistuslinja")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "HAMMASTUS") 		$ulos .= "<option value='HAMMASTUS' $avain_sel[HAMMASTUS]>					".t("Tuotteen hammastus")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "OLETUSPAIKKA") 		$ulos .= "<option value='OLETUSPAIKKA' $avain_sel[OLETUSPAIKKA]>			".t("Tuoteryhm�n tai tuotestatuksen oletuspaikka")."</option>";
		if ($lukitse_laji== "") $ulos .= "</optgroup>";

		if ($lukitse_laji== "") $ulos .= "<optgroup label='".t("Asiakkaiden avainsanat")."'>";
		if ($lukitse_laji== "" or $lukitse_laji == "ASIAKASLUOKKA") 	$ulos .= "<option value='ASIAKASLUOKKA' $avain_sel[ASIAKASLUOKKA]>			".t("Asiakasluokka")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "ASIAKASOSASTO") 	$ulos .= "<option value='ASIAKASOSASTO' $avain_sel[ASIAKASOSASTO]>			".t("Asiakasosasto")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "ASIAKASRYHMA") 		$ulos .= "<option value='ASIAKASRYHMA' $avain_sel[ASIAKASRYHMA]>			".t("Asiakasryhma")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "ASIAKASTILA") 		$ulos .= "<option value='ASIAKASTILA' $avain_sel[ASIAKASTILA]>				".t("Asiakastila")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "PIIRI") 			$ulos .= "<option value='PIIRI' $avain_sel[PIIRI]>							".t("Asiakkaan piiri")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "ASAVAINSANA") 		$ulos .= "<option value='ASAVAINSANA' $avain_sel[ASAVAINSANA]>				".t("Asiakkaan avainsanojen laji")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "EXTASAVAINSANA") 	$ulos .= "<option value='EXTASAVAINSANA' $avain_sel[EXTASAVAINSANA]>		".t("Extranet-asiakkaan avainsanojen laji")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "LAANI") 			$ulos .= "<option value='LAANI' $avain_sel[LAANI]>							".t("L��ni")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "KUNTA") 			$ulos .= "<option value='KUNTA' $avain_sel[KUNTA]>							".t("Kunta")."</option>";
		if ($lukitse_laji== "") $ulos .= "</optgroup>";

		if ($lukitse_laji== "") $ulos .= "<optgroup label='".t("Yhti�n avainsanat")."'>";
		if ($lukitse_laji== "" or $lukitse_laji == "TV") 				$ulos .= "<option value='TV' $avain_sel[TV]>								".t("Tilausvahvistus")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "TOIMVAHVISTUS")		$ulos .= "<option value='TOIMVAHVISTUS' $avain_sel[TOIMVAHVISTUS]>			".t("Toimitusvahvistus")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "KALETAPA") 			$ulos .= "<option value='KALETAPA' $avain_sel[KALETAPA]>					".t("CRM yhteydenottotapa")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "MYSQLALIAS") 		$ulos .= "<option value='MYSQLALIAS' $avain_sel[MYSQLALIAS]>				".t("Tietokantasarakkeen nimialias")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "TOIMITUSTAPA_OS") 	$ulos .= "<option value='TOIMITUSTAPA_OS' $avain_sel[TOIMITUSTAPA_OS]>		".t("Toimitustapa ostolle (kuljetus)")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "KUKAASEMA") 		$ulos .= "<option value='KUKAASEMA' $avain_sel[KUKAASEMA]>					".t("K�yt�j�n asema")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "ALVULK") 			$ulos .= "<option value='ALVULK' $avain_sel[ALVULK]>						".t("Ulkomaan ALV%")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "ALV") 				$ulos .= "<option value='ALV' $avain_sel[ALV]>								".t("ALV%")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "SEURANTA") 			$ulos .= "<option value='SEURANTA' $avain_sel[SEURANTA]>					".t("Tilauksen seurantaluokka")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "TOIMEHTO") 			$ulos .= "<option value='TOIMEHTO' $avain_sel[TOIMEHTO]>					".t("Toimitusehto")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "HENKILO_OSASTO") 	$ulos .= "<option value='HENKILO_OSASTO' $avain_sel[HENKILO_OSASTO]>		".t("Henkil�osasto")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "KVERITTELY") 		$ulos .= "<option value='KVERITTELY' $avain_sel[KVERITTELY]>				".t("Kulunvalvonnan erittely")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "KONEENOSA") 		$ulos .= "<option value='KONEENOSA' $avain_sel[KONEENOSA]>					".t("Koneenosa")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "SAHKTILTUN") 		$ulos .= "<option value='SAHKTILTUN' {$avain_sel['SAHKTILTUN']}>			".t("S�hk�isen tilausliit�nn�n tunnukset")."</option>";
		if ($lukitse_laji== "") $ulos .= "</optgroup>";

		if ($lukitse_laji== "") $ulos .= "<optgroup label='".t("Varaston avainsanat")."'>";
		if ($lukitse_laji== "" or $lukitse_laji == "KERAYSLISTA") 		$ulos .= "<option value='KERAYSLISTA' $avain_sel[KERAYSLISTA]>				".t("Ker�yslista")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "LAHETETYYPPI") 		$ulos .= "<option value='LAHETETYYPPI' $avain_sel[LAHETETYYPPI]>			".t("L�hetetyyppi")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "RAHTIKIRJA") 		$ulos .= "<option value='RAHTIKIRJA' $avain_sel[RAHTIKIRJA]>				".t("Rahtikirjatyyppi")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "TOIMTAPAKV") 		$ulos .= "<option value='TOIMTAPAKV' $avain_sel[TOIMTAPAKV]>				".t("Toimitustavan kieliversio")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "TOIMTAPALP") 		$ulos .= "<option value='TOIMTAPALP' {$avain_sel['TOIMTAPALP']}>			".t("Toimitustavan lajittelupiste")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "TERMINAALIALUE") 	$ulos .= "<option value='TERMINAALIALUE' $avain_sel[TERMINAALIALUE]>		".t("Varaston terminaalialue")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "SSCC")			 	$ulos .= "<option value='SSCC' $avain_sel[SSCC]>							".t("SSCC-koodi")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "PAKKAUSKV") 		$ulos .= "<option value='PAKKAUSKV' $avain_sel[PAKKAUSKV]>					".t("Pakkaustiedon kieliversio")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "INVASTEPAIKKA")		$ulos .= "<option value='INVASTEPAIKKA' {$avain_sel['INVASTEPAIKKA']}>		".t("Inventointiaste-raportin hylk��m�t tuotepaikat")."</option>";
		if ($lukitse_laji== "") $ulos .= "</optgroup>";

		if ($lukitse_laji== "") $ulos .= "<optgroup label='".t("Ty�m��r�ysten avainsanat")."'>";
		if ($lukitse_laji== "" or $lukitse_laji == "TYOM_TYOJONO") 		$ulos .= "<option value='TYOM_TYOJONO' $avain_sel[TYOM_TYOJONO]>			".t("Ty�m��r�ysten ty�jono")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "TYOM_TYOSTATUS") 	$ulos .= "<option value='TYOM_TYOSTATUS' $avain_sel[TYOM_TYOSTATUS]>		".t("Ty�m��r�ysten ty�status")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "TYOM_TYOLINJA") 	$ulos .= "<option value='TYOM_TYOLINJA' $avain_sel[TYOM_TYOLINJA]>			".t("Ty�m��r�ysten ty�linja")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "TYOM_TYOKENTAT") 	$ulos .= "<option value='TYOM_TYOKENTAT' $avain_sel[TYOM_TYOKENTAT]>		".t("Ty�m��r�ysten kent�t")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "TYOM_PRIORIT")	  	$ulos .= "<option value='TYOM_PRIORIT' $avain_sel[TYOM_PRIORIT]>			".t("Ty�m��r�ysten prioriteetti")."</option>";
		if ($lukitse_laji== "") $ulos .= "</optgroup>";

		if ($lukitse_laji== "") $ulos .= "<optgroup label='".t("Viennin avainsanat")."'>";
		if ($lukitse_laji== "" or $lukitse_laji == "KT") 				$ulos .= "<option value='KT' $avain_sel[KT]>								".t("Kauppatapahtuman luonne")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "TULLI") 			$ulos .= "<option value='TULLI' $avain_sel[TULLI]>							".t("Poistumistoimipaikka")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "KM") 				$ulos .= "<option value='KM' $avain_sel[KM]>								".t("Kuljetusmuoto")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "C") 				$ulos .= "<option value='C' $avain_sel[C]>									".t("CHN tietue")."</option>";
		if ($lukitse_laji== "") $ulos .= "</optgroup>";

		if ($lukitse_laji== "") $ulos .= "<optgroup label='".t("Maksuehtojen avainsanat")."'>";
		if ($lukitse_laji== "" or $lukitse_laji == "LASKUKUVAUS") 		$ulos .= "<option value='LASKUKUVAUS' $avain_sel[LASKUKUVAUS]>				".t("Maksuposition kuvaus")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "KARHUVIESTI") 		$ulos .= "<option value='KARHUVIESTI' $avain_sel[KARHUVIESTI]>				".t("Karhuviesti")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "MAKSUEHTOKV")		$ulos .= "<option value='MAKSUEHTOKV' $avain_sel[MAKSUEHTOKV]>				".t("Maksuehdon kieliversio")."</option>";
		if ($lukitse_laji== "") $ulos .= "</optgroup>";

		if ($lukitse_laji== "") $ulos .= "<optgroup label='".t("CRM avainsanat")."'>";
		if ($lukitse_laji== "" or $lukitse_laji == "CRM_ROOLI") 		$ulos .= "<option value='CRM_ROOLI' $avain_sel[CRM_ROOLI]>					".t("Yhteyshenkil�n rooli")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "CRM_SUORAMARKKI") 	$ulos .= "<option value='CRM_SUORAMARKKI' $avain_sel[CRM_SUORAMARKKI]>		".t("Yhteyshenkil�n suoramarkkinointitiedot")."</option>";
		if ($lukitse_laji== "") $ulos .= "</optgroup>";

		if ($lukitse_laji== "") $ulos .= "<optgroup label='".t("Muut avainsanat")."'>";
		if ($lukitse_laji== "" or $lukitse_laji == "VAKIOVIESTI") 		$ulos .= "<option value='VAKIOVIESTI' $avain_sel[VAKIOVIESTI]>				".t("Laskun vakioviesti")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "VAKIOVIESTI_TIL")	$ulos .= "<option value='VAKIOVIESTI_TIL' {$avain_sel['VAKIOVIESTI_TIL']}>	".t("Tilausvahvistuksen vakioviesti")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "KAANTALVVIESTI")	$ulos .= "<option value='KAANTALVVIESTI' $avain_sel[KAANTALVVIESTI]>		".t("Tilauksen/Laskun k��nteisen verotuksen viesti")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "LITETY") 			$ulos .= "<option value='LITETY' $avain_sel[LITETY]>						".t("Liitetiedostotyyppi")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "TIL-LITETY") 		$ulos .= "<option value='TIL-LITETY' ".$avain_sel["TIL-LITETY"].">			".t("Tilauksen liitetiedostotyyppi")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "JAKELULISTA") 		$ulos .= "<option value='JAKELULISTA' $avain_sel[JAKELULISTA]>				".t("Email jakelulista")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "LUETTELO") 			$ulos .= "<option value='LUETTELO' $avain_sel[LUETTELO]>					".t("Luettelotyyppi")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "TRIVITYYPPI") 		$ulos .= "<option value='TRIVITYYPPI' $avain_sel[TRIVITYYPPI]>				".t("Tilausrivin tyyppi")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "LASKUTUS_SAATE")	$ulos .= "<option value='LASKUTUS_SAATE' $avain_sel[LASKUTUS_SAATE]>		".t("Laskun s�hk�postisaatekirje asiakkaalle")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "PALAUTUS_SAATE")	$ulos .= "<option value='PALAUTUS_SAATE' $avain_sel[PALAUTUS_SAATE]>		".t("Palautuksen s�hk�postisaatekirje asiakkaalle")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "TV_LISATIETO") 		$ulos .= "<option value='TV_LISATIETO' $avain_sel[TV_LISATIETO]>			".t("Tilausvahvistuksen lis�tiedot")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "INVEN_LAJI") 		$ulos .= "<option value='INVEN_LAJI' $avain_sel[INVEN_LAJI]>				".t("Inventoinnin laji")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "HAE_JA_SELAA") 		$ulos .= "<option value='HAE_JA_SELAA' $avain_sel[HAE_JA_SELAA]>			".t("Hae ja selaa -ohjelman parametrit")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "PUUTEKOMM") 		$ulos .= "<option value='PUUTEKOMM' $avain_sel[PUUTEKOMM]>					".t("Puuterivin kommentti")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "EXTRANET_TEEMA") 	$ulos .= "<option value='EXTRANET_TEEMA' $avain_sel[EXTRANET_TEEMA]>		".t("Extranet ulkoasuteema")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "SOPIMUS_KENTTA") 	$ulos .= "<option value='SOPIMUS_KENTTA' $avain_sel[SOPIMUS_KENTTA]>		".t("Yll�pitosopimuksen rivien kent�t")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "LABEL") 			$ulos .= "<option value='LABEL' {$avain_sel['LABEL']}>						".t("Myyntitilauksen luokitus")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "OSTOTIL_TILTYYP") 	$ulos .= "<option value='OSTOTIL_TILTYYP' {$avain_sel['OSTOTIL_TILTYYP']}>	".t("Ostotilauksen tilaustyyppi")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "TARJOUS_VAKIOTE") 	$ulos .= "<option value='TARJOUS_VAKIOTE' {$avain_sel['TARJOUS_VAKIOTE']}>	".t("Tarjouksen vakioteksti")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "KATEISOTTO")		$ulos .= "<option value='KATEISOTTO' {$avain_sel['KATEISOTTO']}>			".t("K�teisoton luonne")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "KOHDE_TYYPPI")  	$ulos .= "<option value='KOHDE_TYYPPI' {$avain_sel['KOHDE_TYYPPI']}>        ".t("Kohteen tyyppi")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "MYYNNISTA_OSTO")  	$ulos .= "<option value='MYYNNISTA_OSTO' {$avain_sel['MYYNNISTA_OSTO']}>    ".t("Ostotilauksen k�sittely myyntitilauksella")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "PAIKALLISVARAST")	$ulos .= "<option value='PAIKALLISVARAST' {$avain_sel['PAIKALLISVARAST']}>	".t("Paikallisvarasto")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "HINNAT_CRON") 	 	$ulos .= "<option value='HINNAT_CRON' {$avain_sel['HINNAT_CRON']}>        	".t("Hinnat-cronin aikaleima (vvvv-kk-pp tt:mm:ss)")."</option>";
		if ($lukitse_laji== "" or $lukitse_laji == "TUOTE_EXP_CRON")  	$ulos .= "<option value='TUOTE_EXP_CRON' {$avain_sel['TUOTE_EXP_CRON']}>	".t("Tuote Export-cronin aikaleima (vvvv-kk-pp tt:mm:ss)")."</option>";
		if ($lukitse_laji== "") $ulos .= "</optgroup>";

		if ($lukitse_laji== "") $ulos .= "<optgroup label='".t("Verkkokaupan avainsanat")."'>";
		if ($lukitse_laji== "" or $lukitse_laji == "VERKKOKAULINKKI") $ulos .= "<option value='VERKKOKAULINKKI' $avain_sel[VERKKOKAULINKKI]>".t("Verkkokauppalinkki")."</option>";
		if ($lukitse_laji== "") $ulos .= "</optgroup>";

		$ulos .= "</select></td>";
		$jatko = 0;
	}

	if (mysql_field_name($result, $i) == "nakyvyys") {
		if ($avain_sel["TRY"] == "SELECTED"  or $avain_sel["OSASTO"] == "SELECTED") {
			$sel = array();
			$sel[$trow[$i]] = "SELECTED";

			$ulos = "<td><select name = '$nimi'>";
			$ulos .= "<option value = ''>".t("N�ytet��n verkkokaupassa")."</option>";
			$ulos .= "<option value = 'E' $sel[E]>".t("Ei n�ytet� verkkokaupassa")."</option>";
			$ulos .= "</select></td>";
		}
		elseif ($avain_sel["TYOM_TYOKENTAT"] == "SELECTED") {
			$sel = array();
			$sel[$trow[$i]] = "SELECTED";

			$ulos = "<td><select name = '$nimi'>";
			$ulos .= "<option value = ''>".t("N�ytet��n ty�m��r�yksell�")."</option>";
			$ulos .= "<option value = 'K' $sel[K]>".t("N�ytet��n ty�m��r�yksell� ja laskulla")."</option>";
			$ulos .= "<option value = 'L' $sel[L]>".t("N�ytet��n laskulla")."</option>";
			$ulos .= "<option value = 'S' $sel[S]>".t("Sis�inen tieto")."</option>";
			$ulos .= "</select></td>";
		}
		else {
			$tyyppi = 0;
		}

		$jatko = 0;
	}

	if ($avain_sel['OLETUSPAIKKA'] == 'SELECTED') {

		if (mysql_field_name($result, $i) == "selite") {
			$ulos .= "<td>";
			$ulos .= "<input type='text' name='{$nimi}' size='35' maxlength='65535' value='{$trow[$i]}' />";
			$ulos .= t("Oletuspaikka pit�� antaa muodossa hyllyalue-hyllynro-hyllyvali-hyllytaso");
			$ulos .= "</td>";

			$jatko = 0;
		}

		if (mysql_field_name($result, $i) == "selitetark") {
			$sel = array();
			$sel[$trow[$i]] = "SELECTED";

			$query = "	SELECT selite,
						selitetark
						FROM avainsana
						WHERE yhtio = '{$kukarow['yhtio']}'
						AND laji = 'S'";
			$tuote_status_result = pupe_query($query);

			$ulos = "<td><select name = '$nimi'>";
			$ulos .= "<option value = 'X'>".t("Mik� vain")."</option>";
			while($tuote_status = mysql_fetch_assoc($tuote_status_result)) {
				$ulos .= "<option value = '{$tuote_status['selite']}' {$sel[$tuote_status['selite']]}>".t($tuote_status['selitetark'])."</option>";
			}
			$ulos .= "</select></td>";

			$jatko = 0;
		}

		if (mysql_field_name($result, $i) == "selitetark_2") {
			$ulos = '<td>';
			$ulos .= livesearch_kentta('a', 'TUOTERYHMAHAKU', $nimi, 251, $trow[$i], '', '', '', 'ei');
			$ulos .= '</td>';

			$jatko = 0;
		}
	}

	if (mysql_field_name($result, $i) == "kieli") {

		$kieli = $trow[$i];

		$ulos = "<td><select name='$nimi'>";
		$sel  = "";

		//	n�ytet���n vain kielet joilla on
		if ($perhe > 0) {
			if ($lukitse_laji != "") {
				$laji = $lukitse_laji;
			}
			else {
				$laji = $al_laji;
			}

			$query = "	SELECT group_concat(kieli) kielet
						FROM avainsana
						WHERE yhtio = '$kukarow[yhtio]' and perhe='$perhe' and laji = '$laji'";
			$res = pupe_query($query);
			$kilerow = mysql_fetch_array($res);
			$kielet = $kilerow["kielet"];
		}

		$query  = "SHOW columns from sanakirja";
		$fields =  mysql_query($query);

		while ($apurow = mysql_fetch_array($fields)) {
			if (strlen($apurow[0]) == 2 and (strpos($kielet,$apurow[0]) === false or $apurow[0] == $trow[$i])) {
				$sel = "";
				if ($trow[$i] == $apurow[0]) {
					$sel = "SELECTED";
				}
				elseif ($trow[$i] == "" and $apurow[0] == $yhtiorow["kieli"]) {
					$sel = "SELECTED";
				}
				$ulos .= "<option value='$apurow[0]' $sel>$apurow[0] - ".maa($apurow[0])."</option>";
			}
		}

		$ulos .= "</select></td>";

		$jatko = 0;
	}

	if ($avain_sel["TARJOUS_VAKIOTE"] == "SELECTED" and (mysql_field_name($result, $i) == "selite" or mysql_field_name($result, $i) == "selitetark_2" or mysql_field_name($result, $i) == "selitetark_3" or mysql_field_name($result, $i) == "selitetark_4")) {
		$tyyppi = 0;
	}

	if ($avain_sel["KERAYSLISTA"] == "SELECTED" and mysql_field_name($result, $i) == "selite") {

		$handle = opendir("tilauskasittely");

		$ulos  = "<td><select name='$nimi'>";

		while ($file = readdir($handle)) {
			if (strpos($file,"kerayslista") !== FALSE) {
				$sel = "";

				if ($file == $trow[$i]) {
					$sel = "SELECTED";
				}

				$ulos .= "<option value='$file' $sel>".t($file)."</option>";
			}
		}
		closedir($handle);

		$ulos .= "</select></td>";
		$jatko = 0;
	}

	if ($avain_sel["SAHKTILTUN"] == "SELECTED" and mysql_field_name($result, $i) == "selite") {

		$vresult = hae_yhtion_toimipaikat($kukarow['yhtio']);

		$ulos  = "<td><select name='$nimi'>";
		$ulos .= "<option value='0'>".t("Valitse toimipaikka")."</option>";

		while ($vrow = mysql_fetch_assoc($vresult)) {
			$sel = "";

			if ($vrow["tunnus"] == $trow[$i]) {
				$sel = "SELECTED";
			}

			$ulos .= "<option value='{$vrow["tunnus"]}' $sel>".trim($vrow["nimi"]." ".$vrow["postitp"]." ".$vrow["ovtlisa"])."</option>";
		}

		$ulos .= "</select></td>";
		$jatko = 0;
	}

	if ($avain_sel['OSTOTIL_TILTYYP'] == 'SELECTED' and mysql_field_name($result, $i) == 'selitetark_2') {

		$sel = $trow[$i] != '' ? " selected" : "";

		$ulos = "<td><select name='{$nimi}'>";
		$ulos .= "<option value=''>".t("Ei")."</option>";
		$ulos .= "<option value='x'{$sel}>".t("Kyll�")."</option>";
		$ulos .= "</select></td>";
		$jatko = 0;
	}

	if ($avain_sel["RAHTIKIRJA"] == "SELECTED" and mysql_field_name($result, $i) == "selite") {

		$handle = opendir("tilauskasittely");

		$ulos  = "<td><select name='$nimi'>";

		$ulos .= "<option value=''></option>";

		while ($file = readdir($handle)) {
			if (substr($file,0,10) == 'rahtikirja') {
				$sel = "";

				if ($file == $trow[$i]) {
					$sel = "SELECTED";
				}

				$ulos .= "<option value='$file' $sel>".t($file)."</option>";
			}
		}
		closedir($handle);

		$ulos .= "</select></td>";
		$jatko = 0;
	}

	if ($avain_sel["TARRATYYPPI"] == "SELECTED" and mysql_field_name($result, $i) == "selite") {

		$handle = opendir("tilauskasittely");

		$ulos  = "<td><select name='$nimi'>";

		$ulos .= "<option value=''></option>";

		while ($file = readdir($handle)) {
			if (substr($file,0,16) == 'tilaustuotetarra') {
				$sel = "";

				if ($file == $trow[$i]) {
					$sel = "SELECTED";
				}

				$ulos .= "<option value='$file' $sel>".t($file)."</option>";
			}
		}
		closedir($handle);

		$ulos .= "</select></td>";
		$jatko = 0;
	}

	if ($avain_sel["SARJANUMERON_LI"] == "SELECTED" and mysql_field_name($result, $i) == "selite") {

		$selitteet = array(	'ANKKURIVINSSI','JAAKAAPPI','KONEISTUS','KUVATYYPPI','LAATU','MATERIAALI','MERKKI','MOOTTORINMERKKI','MOOTTORINOHJAUS',
							'PILSSIPUMPPU','SIJAINTI','TIIKKISARJA','TOIMITUSKULUT','TYYNYSARJA','TYYPPI','UUNI','VALONHEITIN','VARIRUNKO','VESSA',
							'RUNKOTYYPPI', 'SPRINKLERI','KILPI');

		$ulos  = "<td><select name='$nimi'>";

		foreach ($selitteet as $selite) {
			if (strtoupper($selite) == strtoupper($trow[$i])) {
				$sel = "SELECTED";
			}
			else {
				$sel = "";
			}

			$ulos .= "<option value='$selite' $sel>$selite</option>";

		}

		$ulos .= "</select></td>";
		$jatko = 0;
	}

	if ($avain_sel['MYYNNISTA_OSTO'] == "SELECTED" and mysql_field_name($result, $i) == "selite") {

		$sel = array($trow[$i] => " selected") + array_fill_keys(array('1','2','3','4','5'), '');

		$ulos  = "<td><select name='{$nimi}'>";
		$ulos .= "<option value='0'>".t("Tilauksen rivit lis�t��n olemassa olevalle ostotilaukselle per toimittaja per k�ytt�j� per p�iv�")."</option>";
		$ulos .= "<option value='1' {$sel[1]}>".t("Tilauksen rivit lis�t��n olemassa olevalle ostotilaukselle per toimittaja per k�ytt�j� per p�iv�, l�hetet��n heti")."</option>";
		$ulos .= "<option value='2' {$sel[2]}>".t("Tilauksen rivit lis�t��n olemassa olevalle ostotilaukselle per toimittaja")."</option>";
		$ulos .= "<option value='3' {$sel[3]}>".t("Tilauksen rivit lis�t��n olemassa olevalle ostotilaukselle per toimittaja, l�hetet��n heti")."</option>";
		$ulos .= "<option value='4' {$sel[4]}>".t("Tilauksen rivit lis�t��n olemassa olevalle ostotilaukselle per toimittaja per varasto per ohjausmerkki")."</option>";
		$ulos .= "<option value='5' {$sel[5]}>".t("Tilauksen rivit lis�t��n olemassa olevalle ostotilaukselle per toimittaja per varasto per ohjausmerkki, l�hetet��n heti")."</option>";
		$ulos .= "</select></td>";
		$jatko = 0;
	}

	if ($avain_sel["ALV"] == "SELECTED" and mysql_field_name($result, $i) == "selitetark") {

		$sel = trim($trow[$i]) != "" ? " SELECTED" : "";

		$ulos  = "<td><select name='{$nimi}'>";
		$ulos .= "<option value=''>".t("T�m� ei ole oletusverokanta")."</option>";
		$ulos .= "<option value='o'{$sel}>".t("T�m� on oletusverokanta")."</option>";
		$ulos .= "</select></td>";
		$jatko = 0;
	}

	if ($avain_sel["SAHKTILTUN"] == "SELECTED" and mysql_field_name($result, $i) == "selitetark") {

		$query = "  SELECT nimi, tunnus
					FROM toimi
					WHERE yhtio = '{$kukarow['yhtio']}'
					AND tyyppi not in ('P','K')";
		$toimi_chk_res = pupe_query($query);

		if (mysql_num_rows($toimi_chk_res) > 0) {

			$ulos = "<td><select name='{$nimi}' ".js_alasvetoMaxWidth($nimi, 300)." onchange='submit();'>";
			$ulos .= "<option value=''>".t("Valitse toimittaja")."</option>";

			while ($row = mysql_fetch_assoc($toimi_chk_res)) {
				$sel = "";

				if ($row["tunnus"] == $trow[$i]) {
					$sel = 'selected';
				}

				$ulos .= "<option value='{$row['tunnus']}' {$sel}>{$row['nimi']}</option>";
			}

			$ulos .= "</select></td>";
		}
		else {
			$ulos = "<td><input type='text' name='{$nimi}' value='{$trow[$i]}'></td>";
		}

		$jatko = 0;
	}

	if ($avain_sel["ALVULK"] == "SELECTED" and mysql_field_name($result, $i) == "selitetark_2") {

		$query = "	SELECT distinct koodi, nimi
					FROM maat
					WHERE nimi != ''
					ORDER BY koodi";
		$vresult = pupe_query($query);
		$ulos = "<td><select name='$nimi'>";

		$ulos .= "<option value = '' ></option>";

		while ($vrow=mysql_fetch_array($vresult)) {
			$sel="";
			if (strtoupper($trow[$i]) == strtoupper($vrow[0])) {
				$sel = "SELECTED";
			}
			elseif($trow[$i] == "" and strtoupper($vrow[0]) == strtoupper($yhtiorow["maa"])) {
				$sel = "SELECTED";
			}
			$ulos .= "<option value = '".strtoupper($vrow[0])."' $sel>".t($vrow[1])."</option>";
		}

		$ulos .= "</select></td>";
		$jatko = 0;
	}

	if ($avain_sel['INVEN_LAJI'] == "SELECTED" and mysql_field_name($result, $i) == "selitetark_2") {

		$ulos = "<td>".livesearch_kentta("mainform", "TILIHAKU", $nimi, 270, $trow[$i], "EISUBMIT")."</td>";

		$jatko = 0;
	}

	if ($avain_sel["TOIMEHTO"] == "SELECTED" and mysql_field_name($result, $i) == "selitetark_3") {

		$sel1 = '';
		$sel2 = '';

		if ($trow[$i] == 'LAHETTAJAN_SOPIMUS') {
			$sel1 = 'SELECTED';
		}
		elseif ($trow[$i] == 'VASTAANOTTAJAN_SOPIMUS') {
			$sel2 = 'SELECTED';
		}

		$ulos  = "<td><select name='$nimi'>";
		$ulos .= "<option value=''>".t("K�ytet��n toimitustavan/asiakkaan oletusrahtisopimusta")."</option>";
		$ulos .= "<option value='LAHETTAJAN_SOPIMUS' $sel1>".t("K�ytet��n l�hett�j�n rahtisopimusta")."</option>";
		$ulos .= "<option value='VASTAANOTTAJAN_SOPIMUS' $sel2>".t("K�ytet��n vastaanottajan rahtisopimusta")."</option>";
		$ulos .= "</select></td>";
		$jatko = 0;
	}

	if ($avain_sel["TOIMEHTO"] == "SELECTED" and mysql_field_name($result, $i) == "selitetark_4") {
		$tyyppi = 0;
		$jatko  = 0;
	}

	if (($avain_sel["KARHUVIESTI"] == "SELECTED" or $avain_sel["TV_LISATIETO"] == "SELECTED") and (mysql_field_name($result, $i) == "selite" or mysql_field_name($result, $i) == "selitetark_2" or mysql_field_name($result, $i) == "selitetark_3")) {
		$tyyppi = 0;
		$jatko  = 0;
	}

	if (($avain_sel["KARHUVIESTI"] == "SELECTED" or $avain_sel["TV_LISATIETO"] == "SELECTED" or $avain_sel['KAANTALVVIESTI'] == "SELECTED" or $avain_sel['TARJOUS_VAKIOTE'] == "SELECTED") and mysql_field_name($result, $i) == "selitetark") {
		$ulos = "<td><textarea rows=10 cols=50 name='$nimi'>$trow[$i]</textarea></td>";
		$jatko = 0;
	}

	if (($avain_sel["OSASTO"] == "SELECTED"  or $avain_sel["TRY"] == "SELECTED") and mysql_field_name($result, $i) == "selitetark_3") {
		$ulos = "<td><textarea rows=10 cols=50 name='$nimi'>$trow[$i]</textarea></td>";
		$jatko = 0;
	}

	if (($avain_sel['LASKUTUS_SAATE'] == "SELECTED" or $avain_sel['PALAUTUS_SAATE'] == "SELECTED" or $avain_sel['KAANTALVVIESTI'] == "SELECTED") and mysql_field_name($result, $i) == "selitetark_2") {
		$ulos = "<td><textarea rows=30 cols=50 name='$nimi'>$trow[$i]</textarea></td>";
		$jatko = 0;
	}

	if (($avain_sel['VAKIOVIESTI'] == "SELECTED" or $avain_sel['VAKIOVIESTI_TIL']) and (mysql_field_name($result, $i) == "selite" or mysql_field_name($result, $i) == "selitetark")) {
		$ulos = "<td><textarea rows=5 cols=50 name='$nimi'>$trow[$i]</textarea></td>";
		$jatko = 0;
	}

	// Tuoteryhmiss� voidaan antaa oletusker�ysprioriteetti selitetark_4 kentt��n
	if ($avain_sel["TRY"] == "SELECTED" and mysql_field_name($result, $i) == "selitetark_4") {

		$vresult = t_avainsana("ASIAKASLUOKKA");

		$ulos = "<td><select name = '$nimi'>";
		$ulos .= "<option value = ''>".t("Ei ker�ysprioriteetti�")."</option>";

		while ($row = mysql_fetch_assoc($vresult)) {
			$sel = ($row["selite"] == $trow[$i]) ? ' selected' : '';
			$ulos .= "<option value = '{$row["selite"]}'$sel>{$row["selite"]} - {$row["selitetark"]}</option>";
		}
		$ulos .= "</select></td>";

		$jatko = 0;
	}

	if (($avain_sel["TYOM_PRIORIT"] == "SELECTED" or $avain_sel["TYOM_TYOJONO"] == "SELECTED" or $avain_sel["TYOM_TYOSTATUS"] == "SELECTED") and mysql_field_name($result, $i) == "selitetark_3") {

		if ($trow[$i] == 'OLETUS') {
			$sel = "SELECTED";
		}

		$ulos = "<td><select name='$nimi'>";
		$ulos .= "<option value='' $sel>".t("Ei oletus")."</option>";
		$ulos .= "<option value='OLETUS' $sel>".t("Oletus")."</option>";
		$ulos .= "</select></td>";
		$jatko = 0;
	}

	if ($avain_sel['TYOM_TYOLINJA'] == "SELECTED" and mysql_field_name($result, $i) == "selite") {

		$query = "	SELECT selite, selitetark
					FROM avainsana
					WHERE yhtio = '$kukarow[yhtio]' and laji = 'TYOM_TYOJONO'
					ORDER BY jarjestys, selitetark_2";
		$vresult = pupe_query($query);

		$ulos = "<td><select name='$nimi'>";
		$ulos .= "<option value = ''>".t("Kaikki ty�jonot")."</option>";

		while ($vrow=mysql_fetch_array($vresult)) {
			$sel="";
			if ($trow[$i] == $vrow[0]) {
				$sel = "selected";
			}

			$ulos .= "<option value = '$vrow[0]' $sel>$vrow[1]</option>";
		}

		$ulos .= "</select></td>";
		$jatko = 0;
	}

	if ($avain_sel['TYOM_TYOLINJA'] == "SELECTED" and mysql_field_name($result, $i) == "selitetark") {

		$query = "	SELECT kuka, nimi
					FROM kuka
					WHERE yhtio = '$kukarow[yhtio]'
					AND aktiivinen = 1
					AND extranet = ''
					ORDER BY nimi";
		$vresult = pupe_query($query);

		$ulos = "<td><select name='$nimi'>";

		while ($vrow=mysql_fetch_array($vresult)) {
			$sel="";
			if ($trow[$i] == $vrow[0]) {
				$sel = "selected";
			}

			$ulos .= "<option value = '$vrow[0]' $sel>$vrow[1]</option>";
		}

		$ulos .= "</select></td>";
		$jatko = 0;
	}

	if ($avain_sel["RAHTIKIRJA"] == "SELECTED" and mysql_field_name($result, $i) == "selitetark_2") {

		$sel = array();
		$sel[$trow[$i]] = "SELECTED";

		$ulos = "<td><select name = '$nimi'>";
		$ulos .= "<option value = ''>".t("Oletus")."</option>";
		$ulos .= "<option value = '1' $sel[1]>".t("Tulostintyyppi")." - ".t("Laser A4")."</option>";
		$ulos .= "<option value = '2' $sel[2]>".t("Tulostintyyppi")." - ".t("Laser A5")."</option>";
		$ulos .= "<option value = '3' $sel[3]>".t("Tulostintyyppi")." - ".t("Matriisi")."</option>";
		$ulos .= "</select></td>";
		$jatko = 0;
	}

	if ($avain_sel['VARASTORYHMA'] == 'SELECTED' and mysql_field_name($result, $i) == "selitetark") {
		$query = "	SELECT tunnus, nimitys
					FROM varastopaikat
					WHERE yhtio = '{$kukarow['yhtio']}' AND tyyppi != 'P'";
		$varasto_res = pupe_query($query);

		$ulos .= "<td>";

		$varastot = explode(",", $trow[$i]);

		while ($varasto_row = mysql_fetch_assoc($varasto_res)) {
			$chk = in_array($varasto_row['tunnus'], $varastot) ? ' checked' : '';
			$ulos .= "<input type='checkbox' name='varastot_array[]' value='$varasto_row[tunnus]'$chk> $varasto_row[nimitys]<br/>";
		}

		$ulos .= "</td>";
		$jatko = 0;
	}

	if ($avain_sel["HAE_JA_SELAA"] == "SELECTED" and mysql_field_name($result, $i) == "selite") {
		$sel = array();
		$sel[$trow[$i]] = "SELECTED";

		$ulos = "<td><select name = '$nimi'>";
		$ulos .= "<option value = 'A' $sel[A]>".t("Hae ja selaa -ohjelmassa k�ytet��n alkuper�ist� ulkoasua")."</option>";
		$ulos .= "<option value = 'B' $sel[B]>".t("Hae ja selaa -ohjelmassa k�ytet��n uutta ulkoasua")."</option>";
		$ulos .= "</select></td>";

		$jatko = 0;
	}

	if (mysql_field_name($result, $i) == "jarjestys") {
		if ($trow[$i] == "" and $ajarjestys != "") {
			$trow[$i] = $ajarjestys;
		}
	}

	if ($avain_sel['KATEISOTTO'] == "SELECTED" and mysql_field_name($result, $i) == "selite") {
		$sel = array();
		$sel[$trow[$i]] = "SELECTED";

		$query = "	SELECT tunnus,
					concat(tilino,' - ',nimi) as dropdown_text
					FROM tili
					WHERE yhtio = '{$kukarow['yhtio']}'";
		$tilires = pupe_query($query);
		$tilit = array();
		while($tili = mysql_fetch_assoc($tilires)) {
			$tilit[] = $tili;
		}

		$ulos = "<td><select name = '$nimi'>";
		$ulos .= "<option value = '' >".t("Valitse tili")."</option>";
		foreach($tilit as $tili) {
			$sel="";
			if ($trow[$i] == $tili['tunnus']) {
				$sel = "selected";
			}
			$ulos .= "<option value = '{$tili['tunnus']}' $sel>".t($tili['dropdown_text'])."</option>";
		}
		$ulos .= "</select></td>";

		$jatko = 0;
	}
