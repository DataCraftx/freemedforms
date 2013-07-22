<?php

    // 0. r�cup�re les arguments
    $userName = $_POST['user'];
    $message = $_POST['msg'];

    // 1. V�rifie l'existence du fichier de l'utilisateur, si absent cr�er le fichier
    $userFileName = 'FMF_' . $userName . '_log.txt';
    if (!file_exists( $userFileName ) )
      {
        touch( $userFileName );
        $fileCreate = fopen ( $userFileName , "r+" );  
        fwrite( $fileCreate, '0' );
        fclose( $fileCreate );
      }

    // 2. Ouvre le fichier de log, incr�mente la valeur et sauvegarde
    $fp = fopen ( $userFileName , "r+" );  
    $nb_commits = fgets ( $fp, 11 );  
    $nb_commits = $nb_commits + 1;  
    fseek ( $fp, 0 );  
    fputs ( $fp, $nb_commits );  
    fclose ( $fp );  

    // 3. Ouvre le fichier des mol�cules v�rifi�es, ajoute les valeurs pass�es en param�tre
    $molsFileName = "FMF_UncorrectMols.txt";

    $fp = fopen ( $molsFileName , "a" );  
    fputs ( $fp, $message );  
    fclose ( $fp );  

    if ( $nb_commits % 20 == 0 )
        if ( mail( 'eric.maeker@free.fr', '[FreeMedForms-FromApp]', $userName . ' -> ' . $nb_commits ) )
           echo 'Mailing author';
        else
           echo 'An error occured while mailing author';
    else
           echo 'Saving data to server';

?>

