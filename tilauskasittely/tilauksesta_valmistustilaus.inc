<?php

	// jos halutaan generoida ostotilaus t�m�n tilauksen riveist�
	// tarvitaan myyntitilauksen tunnus muuttujassa $otunnus

	if (!function_exists("tilauksesta_valmistustilaus")) {
		function tilauksesta_valmistustilaus($otunnus) {
			global $yhtiorow, $kukarow, $tuoteno_array, $kpl_array, $kommentti_array, $lapsenlap_array;

			$tilauksesta_valmistustilaus = "";
			$kukakesken	= $kukarow["kesken"];
			$tehtiin_uusi_otsikko = false;

			$query = "	SELECT *
						FROM lasku
						WHERE yhtio	= '$kukarow[yhtio]'
						and tunnus	= '$otunnus'";
			$result = mysql_query($query) or pupe_error($query);
			$myytilrow = mysql_fetch_array($result);

			//otetaan ensin vain lis�varusteettomat tuotteet tai lis�varusteperheiden is�t
			$query = "	SELECT tilausrivin_lisatiedot.*, tilausrivi.*, jt+varattu kpl, tilausrivi.tunnus rivitunnus
						FROM tilausrivi
						LEFT JOIN tilausrivin_lisatiedot ON (tilausrivin_lisatiedot.yhtio=tilausrivi.yhtio and tilausrivin_lisatiedot.tilausrivitunnus=tilausrivi.tunnus)
						WHERE tilausrivi.yhtio = '$kukarow[yhtio]'
						and tilausrivi.otunnus = '$otunnus'
						and tilausrivi.tyyppi != 'D'
						and tilausrivi.var = 'R'
						and tilausrivi.perheid = 0
						and tilausrivi.perheid2 = 0
						ORDER BY tilausrivi.tunnus";
			$lsresult = mysql_query($query) or pupe_error($query);

			if (mysql_num_rows($lsresult) > 0) {
				while ($valmlisaarivi = mysql_fetch_array($lsresult)) {

					$viesti = trim($myytilrow["tunnusnippu"]." ".$myytilrow["toim_nimi"]);

					// Tarkistetaan onko meill� jo otsikko
					$query = "	SELECT tunnus
								from lasku
								where yhtio 		= '$kukarow[yhtio]'
								and liitostunnus 	= '999999999'
								and tila 			= 'V'
								and tilaustyyppi 	= 'W'
								and alatila 		= ''
								and toim_nimi		= '$myytilrow[toim_nimi]'
								and toim_nimitark	= '$myytilrow[toim_nimitark]'
								and toim_osoite		= '$myytilrow[toim_osoite]'
								and toim_postino	= '$myytilrow[toim_postino]'
								and toim_postitp	= '$myytilrow[toim_postitp]'
								and toim_maa		= '$myytilrow[toim_maa]'
								and toimaika		= '$myytilrow[toimaika]'
								and kerayspvm		= '$myytilrow[kerayspvm]'
								and viesti			= '$viesti'
								and left(luontiaika,10) = left(now(),10)
								and laatija 		= '$kukarow[kuka]'";
					$jtsre = mysql_query($query) or pupe_error($query);

					// ei l�ydy, tehd��n uus otsikko
					if (mysql_num_rows($jtsre) == 0) {

						$query = "	SELECT *
									FROM varastopaikat
									WHERE yhtio = '$kukarow[yhtio]'
									and tyyppi != 'E'
									ORDER BY tyyppi, nimitys
									LIMIT 1";
						$vtresult = mysql_query($query) or pupe_error($query);
						$vrow = mysql_fetch_array($vtresult);

						$jatka 				= TRUE;
						$toim				= "VALMISTAVARASTOON";
						$kukarow["kesken"]	= 0;
						$clearing 			= $vrow["tunnus"];
						$comments 			= $viesti;
						$eilahe 			= "";
						$viesti 			= $viesti;
						$alatila			= "";
						$varasto 			= "";
						$luokka 			= "";
						$tunnusnippu		= $myytilrow["tunnusnippu"];
						$toimitustapa		= "";
						$tilausnumero 		= 0;
						$nokeskupdate 		= TRUE;
						$toim_nimi			= $myytilrow['toim_nimi'];
						$toim_nimitark		= $myytilrow['toim_nimitark'];
						$toim_osoite		= $myytilrow['toim_osoite'];
						$toim_postino		= $myytilrow['toim_postino'];
						$toim_postitp		= $myytilrow['toim_postitp'];
						$toim_maa			= $myytilrow['toim_maa'];

						if ($yhtiorow["kerataanko_valmistukset"] == "E" and ($toim == "VALMISTAASIAKKAALLE" or $toim == "VALMISTAVARASTOON")) {
							$eilahe = 'CHECKED';
						}

						list($toimvv, $toimkk, $toimpp) = explode('-', $myytilrow["toimaika"]);
						list($kervv, $kerkk, $kerpp)    = explode('-', $myytilrow["kerayspvm"]);

						require("tilauskasittely/otsik_siirtolista.inc");

						$tunnus = $id;
						$tehtiin_uusi_otsikko = true;

						$tilauksesta_valmistustilaus = "<font class='message'>".t("Perustettiin valmistus %s", $kukarow["kieli"], $tunnus)."</font><br>";
					}
					else {
						// tilaus l�yty, otetaan tunnus
						$jtsro = mysql_fetch_array($jtsre);
						$tunnus = $jtsro["tunnus"];
					}

					if (!isset($laskurow)) {
						$query = "	SELECT *
									FROM lasku
									WHERE yhtio	= '$kukarow[yhtio]'
									and tunnus	= '$tunnus'";
						$lresult = mysql_query($query) or pupe_error($query);
						$laskurow = mysql_fetch_array($lresult);
					}

					// $myy_sarjatunnus  --> Jos halutaan automaattisesti linkata joku sarjanumero-olio tilausriviin
					// $osto_sarjatunnus --> Jos halutaan automaattisesti linkata joku sarjanumero-olio tilausriviin

					//funktio populoi globaalit muuttujat $tuoteno_array $kpl_array $kommentti_array $lapsenlap_array
					$tuoteno_array = array($valmlisaarivi['tuoteno']);
					$kpl_array = array(
						$valmlisaarivi['tuoteno'] => $valmlisaarivi['kpl']
					);

					pupesoft_lisaa_valmisteen_rekursiiviset_reseptit();

					foreach ($tuoteno_array as $tuoteno_temp) {
						$kukarow["kesken"] 	= $tunnus;
						$kpl 				= $kpl_array[$tuoteno_temp];
						$tuoteno			= $tuoteno_temp;
						$toimaika			= $myytilrow["kerayspvm"];
						$kerayspvm			= $myytilrow["kerayspvm"];
						$hinta				= 0;
						$var 				= "";
						$varasto			= "";
						$paikka				= "";
						$varataan_saldoa	= "EI";
						$kommentti			= $kommentti_array[$tuoteno_temp];

						$query = "	SELECT *
									FROM tuote
									WHERE yhtio = '$kukarow[yhtio]'
									AND tuoteno = '$tuoteno'";
						$tuoteres = pupe_query($query);
						$trow = mysql_fetch_assoc($tuoteres);

						require('lisaarivi.inc');
					}

					// Ainoastaan asiakkaalle valmistetut j�lkitoimitusrivit kohdistetaan ja toimitetaan automaattisesti valmistuksen yhteydess�
					$suoraan_laskutukseen_lisa = $yhtiorow['automaattinen_jt_toimitus_valmistus'] == 'S' ? 'o' : '';

					//	P�ivitet��n tilausrivin lisatietoihin tilausrivitunnus
					$query = "	UPDATE tilausrivin_lisatiedot SET
									tilausrivilinkki 		= '$perheid_lisaarivi',
									suoraan_laskutukseen	= '{$suoraan_laskutukseen_lisa}',
									muutospvm				= now(),
									muuttaja				= '$kukarow[kuka]'
								WHERE yhtio	= '$kukarow[yhtio]' and tilausrivitunnus = '$valmlisaarivi[tunnus]'";
					$updres = mysql_query($query) or pupe_error($query);

					//Kopioidaan otsikon lisatiedot
					if ($tehtiin_uusi_otsikko) {
						$query = "SELECT * FROM laskun_lisatiedot WHERE otunnus='$otunnus' and yhtio ='$kukarow[yhtio]'";
						$lisatiedotres = mysql_query($query) or pupe_error($query);
						$lisatiedotrow = mysql_fetch_array($lisatiedotres);

						$fields = mysql_field_name($lisatiedotres,0);
						$values = "'".$lisatiedotrow[0]."'";

						for($i=1; $i < mysql_num_fields($lisatiedotres)-1; $i++) { // Ei monisteta tunnusta

							$fields .= ", ".mysql_field_name($lisatiedotres,$i);

							switch (mysql_field_name($lisatiedotres,$i)) {
								case 'otunnus':
									$values .= ", '$tunnus'";
									break;
								default:
									$values .= ", '".$lisatiedotrow[$i]."'";
							}
						}

						$kysely  = "INSERT into laskun_lisatiedot ($fields) VALUES ($values)";
						$insres2 = mysql_query($kysely) or pupe_error($kysely);

						$tehtiin_uusi_otsikko = false;
					}

					//	P�ivitet��n my�s tilausriville oikeammat tiedot!
					$query = "	SELECT *
								FROM tilausrivin_lisatiedot
								WHERE tilausrivitunnus='$valmlisaarivi[tunnus]' and yhtio ='$kukarow[yhtio]'";
					$trlisatiedotres = mysql_query($query) or pupe_error($query);
					$trlisatiedotrow = mysql_fetch_array($trlisatiedotres);

					$values = mysql_field_name($trlisatiedotres,0)." = '".$trlisatiedotrow[0]."'";

					for($i=1; $i < mysql_num_fields($trlisatiedotres)-1; $i++) { // Ei monisteta tunnusta

						switch (mysql_field_name($trlisatiedotres,$i)) {
							case 'tilausrivitunnus':
								$values .= ", ".mysql_field_name($trlisatiedotres,$i)." = '$perheid_lisaarivi'";
								break;
							case 'tilausrivilinkki':
								$values .= ", ".mysql_field_name($trlisatiedotres,$i)." = ''";
								break;
							case 'pituus':
								$pituus = $trlisatiedotrow[$i];
								$uusiPituus = $pituus;
								$values .= ", ".mysql_field_name($trlisatiedotres,$i)." = '1000'";
								break;
							default:
								$values .= ", ".mysql_field_name($trlisatiedotres,$i)." = '".$trlisatiedotrow[$i]."'";
						}
					}

					$kysely  = "UPDATE tilausrivin_lisatiedot SET $values WHERE yhtio='$kukarow[yhtio]' and tilausrivitunnus = '$perheid_lisaarivi'";
					$insres2 = mysql_query($kysely) or pupe_error($kysely);

					//	Tehd��n rivist� normi JT
					$query = "UPDATE tilausrivi set var='J' where yhtio='$kukarow[yhtio]' and tunnus='$valmlisaarivi[tunnus]'";
					$updre = mysql_query($query) or pupe_error($query);
				}
			}

			$kukarow["kesken"] = $kukakesken;

			return $tilauksesta_valmistustilaus;
		}
	}
?>
