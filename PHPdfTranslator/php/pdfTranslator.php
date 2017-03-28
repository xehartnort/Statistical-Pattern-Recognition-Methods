<?php
require 'vendor/autoload.php';

use Stichoza\GoogleTranslate\TranslateClient;
use \Smalot\PdfParser\Parser;

$file = '../a.pdf';
$tr = new TranslateClient('pl', 'en');
$parser = new Parser();
$pdf_pages = $parser->parseFile($file)->getPages();
foreach( $pdf_pages as $key => $value )
 {
 	try 
 	{
 		echo "\n\n"."Page ";
		echo $tr->translate($value->getText());
	}
	catch(Exception $e)
	{
		$pag = $key+1;
		echo "Skipped page ".$pag."\n";
	}

 }
?>