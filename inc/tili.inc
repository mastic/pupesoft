<?php
	$otsikko = 'Tilikartta';
	$kentat = "	tunnus, tilino, nimi, 
				sisainen_taso,
				(select group_concat(nimi) from taso where yhtio='$kukarow[yhtio]' and tyyppi='S' and taso = BINARY sisainen_taso) sisainen_nimi, 
				ulkoinen_taso,
				(select group_concat(nimi) from taso where yhtio='$kukarow[yhtio]' and tyyppi='U' and taso = BINARY ulkoinen_taso) ulkoinen_nimi,
				alv_taso,
				(select group_concat(nimi) from taso where yhtio='$kukarow[yhtio]' and tyyppi='A' and taso = BINARY alv_taso) alv_nimi";
	$jarjestys = 'tilino';
?>