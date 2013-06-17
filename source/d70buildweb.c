/************************************************************************/
/*      $Id: d70buildweb.c,v 1.4 2005/04/14 17:37:07 joman Exp joman $  */
/*                                                                      */
/*      Source file name:   d70buildweb.c                               */
/*                                                                      */
/************************************************************************/
/*      Revisions:                                                      */
/*                                                                      */
/*      Version 0.01 - Mon Mar 21 17:15:52 EST 2005:                    */
/*              First coding.                                           */
/*      Version 0.02 - Wed Mar 23 22:54:24 EST 2005:                    */
/*              Corrected a problem, where the program was not          */
/*              rotating a JPG image with the -web option.              */
/*      Version 0.03 - Thu Apr 14 13:17:57 EDT 2005:                    */
/*		Made changes to support the correct handling of		*/
/*		vertical images.					*/
/*									*/
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#include <io.h>
#endif

#include "d70reader.h"
#include "d70inc2.h"

#ifdef MPATROL_DEBUG
#include "mpatrol.h"
#endif

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void create_css(char * filename)
{
    FILE *ofd;

	if((ofd = fopen(filename, "w")) == NULL)
	{
		printf("\nFile not opened: %s\n\n", filename);
		return;
	}

	fputs("DIV {", ofd);
	fputs("\n	BACKGROUND: black; COLOR: #888888; FONT-FAMILY: arial,helvetica,sans-serif", ofd);
	fputs("\n}", ofd);
	fputs("\nBODY {", ofd);
	fputs("\n	BACKGROUND: black; COLOR: #888888; FONT-FAMILY: arial,helvetica,sans-serif", ofd);
	fputs("\n}", ofd);
	fputs("\n#slideshow1 TD {", ofd);
	fputs("\n	TEXT-ALIGN: right", ofd);
	fputs("\n}", ofd);
	fputs("\n#slideshow1 A {", ofd);
	fputs("\n	FONT-WEIGHT: bold", ofd);
	fputs("\n}", ofd);
	fputs("\n#slideshow2 TD {", ofd);
	fputs("\n	TEXT-ALIGN: right", ofd);
	fputs("\n}", ofd);
	fputs("\n#slideshow2 A {", ofd);
	fputs("\n	FONT-WEIGHT: bold", ofd);
	fputs("\n}", ofd);
	fputs("\nA:active {", ofd);
	fputs("\n	COLOR: #d5ae83", ofd);
	fputs("\n}", ofd);
	fputs("\nA:visited {", ofd);
	fputs("\n	COLOR: #5068e3", ofd);
	fputs("\n}", ofd);
	fputs("\nA:hover {", ofd);
	fputs("\n	COLOR: #5b80b7", ofd);
	fputs("\n}", ofd);
	fputs("\n.title {", ofd);
	fputs("\n	FONT-WEIGHT: bold; FONT-SIZE: 14pt; TEXT-ALIGN: center", ofd);
	fputs("\n}", ofd);
	fputs("\nTD.thumbnail {", ofd);
	fputs("\n	VERTICAL-ALIGN: top", ofd);
	fputs("\n}", ofd);
	fputs("\nH1 {", ofd);
	fputs("\n	BACKGROUND: black; COLOR: #888888; FONT-FAMILY: arial,helvetica,sans-serif", ofd);
	fputs("\n}", ofd);
	fputs("\nH2 {", ofd);
	fputs("\n	BACKGROUND: black; COLOR: #888888; FONT-FAMILY: arial,helvetica,sans-serif", ofd);
	fputs("\n}", ofd);
	fputs("\nH3 {", ofd);
	fputs("\n	BACKGROUND: black; COLOR: #888888; FONT-FAMILY: arial,helvetica,sans-serif", ofd);
	fputs("\n}", ofd);
	fputs("\nH4 {", ofd);
	fputs("\n	BACKGROUND: black; COLOR: #888888; FONT-FAMILY: arial,helvetica,sans-serif", ofd);
	fputs("\n}", ofd);
	fputs("\nH5 {", ofd);
	fputs("\n	BACKGROUND: black; COLOR: #888888; FONT-FAMILY: arial,helvetica,sans-serif", ofd);
	fputs("\n}", ofd);
	fputs("\n", ofd);

	fclose(ofd);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void create_root_index(char * filename)
{
    FILE *ofd;

	if((ofd = fopen(filename, "w")) == NULL)
	{
		printf("\nFile not opened: %s\n\n", filename);
		return;
	}

    fputs("<!DOCTYPE html", ofd);
    fputs("\nPUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"", ofd);
    fputs("\n\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">", ofd);
    fputs("\n", ofd);
    fputs("\n<html xmlns=\"http://www.w3.org/1999/xhtml\">", ofd);
    fputs("\n", ofd);
    fputs("\n<head><title>D70Reader Gallery Selection</title>", ofd);
    fputs("\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1252\" />", ofd);
    fputs("\n<link href=\"gallery.css\" type=\"text/css\" rel=\"stylesheet\" />", ofd);
    fputs("\n", ofd);
    fputs("\n<script src=\"images.js\" type=\"text/javascript\"></script>", ofd);
    fputs("\n", ofd);
    fputs("\n<script language=\"javascript\" type=\"text/javascript\">", ofd);
    fputs("\n<!--", ofd);
    fputs("\nvar charTableData = \"\";", ofd);
    fputs("\nvar intTagIndex;", ofd);
    fputs("\nvar intExifImageWidth;", ofd);
    fputs("\nvar intExifImageLength;", ofd);
    fputs("\nvar intExifImageWidthIndex;", ofd);
    fputs("\nvar intExifImageLengthIndex;", ofd);
    fputs("\n", ofd);
    fputs("\nfunction getTagIndex(taglabel) {", ofd);
    fputs("\n", ofd);
    fputs("\n    for (k=0; k<aExifDataHeader[0].length; k++)", ofd);
    fputs("\n    {", ofd);
    fputs("\n        if(aExifDataHeader[0][k] == taglabel)", ofd);
    fputs("\n        {", ofd);
    fputs("\n            return k;", ofd);
    fputs("\n        }", ofd);
    fputs("\n    }", ofd);
    fputs("\n", ofd);
    fputs("\n    return -1;", ofd);
    fputs("\n}", ofd);
    fputs("\n", ofd);
    fputs("\nintOrientationIndex = getTagIndex(\"Orientation\");", ofd);
    fputs("\nintExifImageWidthIndex = getTagIndex(\"ExifImageWidth\");", ofd);
    fputs("\nintExifImageLengthIndex = getTagIndex(\"ExifImageLength\");", ofd);
    fputs("\n", ofd);
    fputs("\n//-->", ofd);
    fputs("\n</script>", ofd);
    fputs("\n", ofd);
    fputs("\n</head>", ofd);
    fputs("\n<body>", ofd);
    fputs("\n<h2>D70Reader Gallery Selection</h2>", ofd);
    fputs("\n<div class=\"thumbnails\" align=\"center\">", ofd);
    fputs("\n", ofd);
    fputs("\n<script type=\"text/javascript\">", ofd);
    fputs("\n<!--", ofd);
    fputs("\n    charTableData += '<table cellspacing=\"15\" cellpadding=\"15\" width=\"85%\" border=\"0\">';", ofd);
    fputs("\n    charTableData += '  <tbody>';", ofd);
    fputs("\n", ofd);
    fputs("\n    for (ctr1=0; ctr1<=intExifDataArrayCount; ctr1+=3)", ofd);
    fputs("\n    {", ofd);
    fputs("\n        intExifImageWidth = aExifData[ctr1][intExifImageWidthIndex]/4;", ofd);
    fputs("\n        intExifImageLength= aExifData[ctr1][intExifImageLengthIndex]/4;", ofd);
    fputs("\n", ofd);
    fputs("\n        charTableData += '<tr valign=\"center\">';", ofd);
    fputs("\n        charTableData += '<td class=\"thumbnail\" align=\"middle\"> <a class=\"thumbnail\" ';", ofd);
    fputs("\n        charTableData += 'href=\"gallery' + (ctr1 + 1) + '/index.html\">';", ofd);
    fputs("\n        charTableData += '<img class=\"thumbnail\" height=\"';", ofd);
    fputs("\n        charTableData += intExifImageLength;        ", ofd);
    fputs("\n        charTableData += '\" alt=\"Gallery#' + (ctr1 + 1) + '\"';", ofd);
    fputs("\n        charTableData += 'src=\"gallery' + (ctr1 + 1) + '/images/thumbnail/' + aExifData[ctr1][1] + '\" ';", ofd);
    fputs("\n        charTableData += 'width=\"';", ofd);
    fputs("\n        charTableData += intExifImageWidth;        ", ofd);
    fputs("\n        charTableData += '\" border=\"0\"></a><br><font size=\"3\">Gallery #' + (ctr1 + 1) + '</font></td>';", ofd);
    fputs("\n", ofd);
    fputs("\n        if((ctr1 + 1) <= intExifDataArrayCount)", ofd);
    fputs("\n        {", ofd);
    fputs("\n            intExifImageWidth = aExifData[ctr1 + 1][intExifImageWidthIndex]/4;", ofd);
    fputs("\n            intExifImageLength= aExifData[ctr1 + 1][intExifImageLengthIndex]/4;", ofd);
    fputs("\n", ofd);
    fputs("\n            charTableData += '<td class=\"thumbnail\" align=\"middle\"><a class=\"thumbnail\" ';", ofd);
    fputs("\n            charTableData += 'href=\"gallery' + (ctr1 + 2) + '/index.html\">';", ofd);
    fputs("\n            charTableData += '<img class=\"thumbnail\" height=\"';", ofd);
    fputs("\n            charTableData += intExifImageLength;        ", ofd);
    fputs("\n            charTableData += '\" alt=\"Gallery#' + (ctr1 + 2) + ' \"';", ofd);
    fputs("\n            charTableData += 'src=\"gallery' + (ctr1 + 2) + '/images/thumbnail/' + aExifData[ctr1 + 1][1] + '\" ';", ofd);
    fputs("\n            charTableData += 'width=\"';", ofd);
    fputs("\n            charTableData += intExifImageWidth;        ", ofd);
    fputs("\n            charTableData += '\" border=\"0\"></a><br><font size=\"3\">Gallery #' + (ctr1 + 2) + '</font></td>';", ofd);
    fputs("\n        }", ofd);
    fputs("\n", ofd);
    fputs("\n        if((ctr1 + 2) <= intExifDataArrayCount)", ofd);
    fputs("\n        {", ofd);
    fputs("\n            intExifImageWidth = aExifData[ctr1 + 2][intExifImageWidthIndex]/4;", ofd);
    fputs("\n            intExifImageLength= aExifData[ctr1 + 2][intExifImageLengthIndex]/4;", ofd);
    fputs("\n", ofd);
    fputs("\n            charTableData += '<td class=\"thumbnail\" align=\"middle\"><a class=\"thumbnail\" ';", ofd);
    fputs("\n            charTableData += 'href=\"gallery' + (ctr1 + 3) + '/index.html\">';", ofd);
    fputs("\n            charTableData += '<img class=\"thumbnail\" height=\"';", ofd);
    fputs("\n            charTableData += intExifImageLength;        ", ofd);
    fputs("\n            charTableData += '\" alt=\"Gallery#' + (ctr1 + 3) + ' \"';", ofd);
    fputs("\n            charTableData += 'src=\"gallery' + (ctr1 + 3) + '/images/thumbnail/' + aExifData[ctr1 + 2][1] + '\" ';", ofd);
    fputs("\n            charTableData += 'width=\"';", ofd);
    fputs("\n            charTableData += intExifImageWidth;        ", ofd);
    fputs("\n            charTableData += '\" border=\"0\"></a><br><font size=\"3\">Gallery #' + (ctr1 + 3) + '</font></td>';", ofd);
    fputs("\n        }", ofd);
    fputs("\n", ofd);
    fputs("\n        charTableData += '</tr>';", ofd);
    fputs("\n    }", ofd);
    fputs("\n", ofd);
    fputs("\n    charTableData += '</tbody></table>';", ofd);
    fputs("\n", ofd);
    fputs("\n    document.write(charTableData);", ofd);
    fputs("\n//-->", ofd);
    fputs("\n</script>", ofd);
    fputs("\n", ofd);
    fputs("\n</div>", ofd);
    fputs("\n<font size=\"small\"><i>click on thumbnails for gallery index</i></font>", ofd);
    fputs("\n</body>", ofd);
    fputs("\n</html>", ofd);

	fclose(ofd);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void create_gallery_index(char * filename, int iGalleryNumber)
{
    FILE *ofd;
	char *str_ptr = NULL;

	if((ofd = fopen(filename, "w")) == NULL)
	{
		printf("\nFile not opened: %s\n\n", filename);
		return;
	}

    fputs("<!DOCTYPE html", ofd);
    fputs("\nPUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"", ofd);
    fputs("\n\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">", ofd);
    fputs("\n", ofd);
    fputs("\n<html xmlns=\"http://www.w3.org/1999/xhtml\">", ofd);
    fputs("\n", ofd);
    fputs("\n<head><title>D70Reader Gallery</title>", ofd);
    fputs("\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1252\" />", ofd);
    fputs("\n<link href=\"../gallery.css\" type=\"text/css\" rel=\"stylesheet\" />", ofd);
    fputs("\n", ofd);
    fputs("\n<script src=\"images.js\" type=\"text/javascript\"></script>", ofd);
    fputs("\n", ofd);
    fputs("\n<script type=\"text/javascript\">", ofd);
    fputs("\n<!--", ofd);
    fputs("\nvar charTableData = \"\";", ofd);
    fputs("\nvar intTagIndex;", ofd);
    fputs("\nvar intExifImageWidth;", ofd);
    fputs("\nvar intExifImageLength;", ofd);
    fputs("\nvar intExifImageWidthIndex;", ofd);
    fputs("\nvar intExifImageLengthIndex;", ofd);
    fputs("\n", ofd);
    fputs("\nfunction getTagIndex(taglabel) {", ofd);
    fputs("\n", ofd);
    fputs("\n    for (k=0; k<aExifDataHeader[0].length; k++)", ofd);
    fputs("\n    {", ofd);
    fputs("\n        if(aExifDataHeader[0][k] == taglabel)", ofd);
    fputs("\n        {", ofd);
    fputs("\n            return k;", ofd);
    fputs("\n        }", ofd);
    fputs("\n    }", ofd);
    fputs("\n", ofd);
    fputs("\n    return -1;", ofd);
    fputs("\n}", ofd);
    fputs("\n", ofd);
    fputs("\nintOrientationIndex = getTagIndex(\"Orientation\");", ofd);
    fputs("\nintExifImageWidthIndex = getTagIndex(\"ExifImageWidth\");", ofd);
    fputs("\nintExifImageLengthIndex = getTagIndex(\"ExifImageLength\");", ofd);
    fputs("\n", ofd);
    fputs("\n//-->", ofd);
    fputs("\n</script>", ofd);
    fputs("\n", ofd);
    fputs("\n</head>", ofd);
    fputs("\n<body>", ofd);

	str_ptr = (char *) my_malloc("str_ptr", strlen("\n<h2>Gallery #%d</h2>") + 10);
	memset(str_ptr, (int) NULL, (strlen("\n<h2>Gallery #%d</h2>") + 10));
	sprintf(str_ptr, "\n<h2>Gallery #%d</h2>", iGalleryNumber);
	fputs(str_ptr, ofd);
	my_free("str_ptr", str_ptr);

    fputs("\n", ofd);
    fputs("\n<div class=\"slideshow\" id=\"indexlink1\">", ofd);
    fputs("\n<table cellspacing=\"1\" width=\"100%\" border=\"1\">", ofd);
    fputs("\n  <tbody>", ofd);
    fputs("\n  <tr>", ofd);
    fputs("\n    <th nowrap=\"nowrap\" align=\"center\">", ofd);
    fputs("\n      <a href=\"../index.html\">Back to Gallery Index</a>", ofd);
    fputs("\n    </th>", ofd);
    fputs("\n  </tr>", ofd);
    fputs("\n  </tbody>", ofd);
    fputs("\n</table>", ofd);
    fputs("\n</div>", ofd);
    fputs("\n", ofd);
    fputs("\n<div class=\"thumbnails\" align=\"center\">", ofd);
    fputs("\n", ofd);
    fputs("\n<script type=\"text/javascript\">", ofd);
    fputs("\n<!--", ofd);
    fputs("\n    charTableData += '<table cellSpacing=\"15\" cellPadding=\"15\" width=\"85%\" border=\"0\">';", ofd);
    fputs("\n", ofd);
    fputs("\n    charTableData += '  <tbody>';", ofd);
    fputs("\n", ofd);
    fputs("\n    for (ctr1=0; ctr1<=intExifDataArrayCount; ctr1+=3)", ofd);
    fputs("\n    {", ofd);
    fputs("\n        intExifImageWidth = aExifData[ctr1][intExifImageWidthIndex]/4;", ofd);
    fputs("\n        intExifImageLength= aExifData[ctr1][intExifImageLengthIndex]/4;", ofd);
    fputs("\n", ofd);
    fputs("\n        charTableData += '<tr valign=\"center\">';", ofd);
    fputs("\n", ofd);
    fputs("\n        charTableData += '<td class=\"thumbnail\" align=\"middle\"><a class=\"thumbnail\" ';", ofd);
    fputs("\n", ofd);
    fputs("\n        charTableData += 'href=\"photos.html?myimage=' + aExifData[ctr1][1] + '\">';", ofd);
    fputs("\n", ofd);
    fputs("\n        charTableData += '<img class=\"thumbnail\" height=\"';", ofd);
    fputs("\n", ofd);
    fputs("\n        charTableData += intExifImageLength;        ", ofd);
    fputs("\n", ofd);
    fputs("\n        charTableData += '\" alt=\"' + aExifData[ctr1][1] + '\"';", ofd);
    fputs("\n", ofd);
    fputs("\n        charTableData += 'src=\"images/thumbnail/' + aExifData[ctr1][1] + '\" ';", ofd);
    fputs("\n", ofd);
    fputs("\n        charTableData += 'width=\"';", ofd);
    fputs("\n", ofd);
    fputs("\n        charTableData += intExifImageWidth;        ", ofd);
    fputs("\n", ofd);
    fputs("\n        charTableData += '\" border=\"0\"></a><br><font size=\"3\">' + aExifData[ctr1][1] + '</font></td>';", ofd);
    fputs("\n", ofd);
    fputs("\n        if((ctr1 + 1) <= intExifDataArrayCount)", ofd);
    fputs("\n        {", ofd);
    fputs("\n            intExifImageWidth = aExifData[(ctr1 + 1)][intExifImageWidthIndex]/4;", ofd);
    fputs("\n            intExifImageLength= aExifData[(ctr1 + 1)][intExifImageLengthIndex]/4;", ofd);
    fputs("\n            charTableData += '<td class=\"thumbnail\" align=\"middle\"><a class=\"thumbnail\" ';", ofd);
    fputs("\n", ofd);
    fputs("\n            charTableData += 'href=\"photos.html?myimage=' + aExifData[ctr1 + 1][1] + '\">';", ofd);
    fputs("\n", ofd);
    fputs("\n            charTableData += '<img class=\"thumbnail\" height=\"';", ofd);
    fputs("\n", ofd);
    fputs("\n            charTableData += intExifImageLength;        ", ofd);
    fputs("\n", ofd);
    fputs("\n            charTableData += '\" alt=\"' + aExifData[ctr1 + 1][1] + '\" ';", ofd);
    fputs("\n", ofd);
    fputs("\n            charTableData += 'src=\"images/thumbnail/' + aExifData[ctr1 + 1][1] + '\" ';", ofd);
    fputs("\n", ofd);
    fputs("\n            charTableData += 'width=\"';", ofd);
    fputs("\n", ofd);
    fputs("\n            charTableData += intExifImageWidth;        ", ofd);
    fputs("\n", ofd);
    fputs("\n            charTableData += '\" border=\"0\"></a><br><font size=\"3\">' + aExifData[ctr1 + 1][1] + '</font></td>';", ofd);
    fputs("\n        }", ofd);
    fputs("\n", ofd);
    fputs("\n        if((ctr1 + 2) <= intExifDataArrayCount)", ofd);
    fputs("\n        {", ofd);
    fputs("\n            intExifImageWidth = aExifData[(ctr1 + 2)][intExifImageWidthIndex]/4;", ofd);
    fputs("\n            intExifImageLength= aExifData[(ctr1 + 2)][intExifImageLengthIndex]/4;", ofd);
    fputs("\n            charTableData += '<td class=\"thumbnail\" align=\"middle\"><a class=\"thumbnail\" ';", ofd);
    fputs("\n", ofd);
    fputs("\n            charTableData += 'href=\"photos.html?myimage=' + aExifData[ctr1 + 2][1] + '\">';", ofd);
    fputs("\n", ofd);
    fputs("\n            charTableData += '<img class=\"thumbnail\" height=\"';", ofd);
    fputs("\n", ofd);
    fputs("\n            charTableData += intExifImageLength;        ", ofd);
    fputs("\n", ofd);
    fputs("\n            charTableData += '\" alt=\"' + aExifData[ctr1 + 2][1] + '\" ';", ofd);
    fputs("\n", ofd);
    fputs("\n            charTableData += 'src=\"images/thumbnail/' + aExifData[ctr1 + 2][1] + '\" ';", ofd);
    fputs("\n", ofd);
    fputs("\n            charTableData += 'width=\"';", ofd);
    fputs("\n", ofd);
    fputs("\n            charTableData += intExifImageWidth;        ", ofd);
    fputs("\n", ofd);
    fputs("\n            charTableData += '\" border=\"0\"></a><br><font size=\"3\">' + aExifData[ctr1 + 2][1] + '</font></td>';", ofd);
    fputs("\n        }", ofd);
    fputs("\n", ofd);
    fputs("\n        charTableData += '</tr>';", ofd);
    fputs("\n    }", ofd);
    fputs("\n", ofd);
    fputs("\n    charTableData += '</tbody></table>';", ofd);
    fputs("\n", ofd);
    fputs("\n    document.write(charTableData);", ofd);
    fputs("\n//-->", ofd);
    fputs("\n</script>", ofd);
    fputs("\n", ofd);
    fputs("\n</div>", ofd);
    fputs("\n<font size=\"small\"><i>click on thumbnails for full image</i></font>", ofd);
    fputs("\n", ofd);
    fputs("\n<div class=\"slideshow\" id=\"indexlink2\">", ofd);
    fputs("\n<table cellspacing=\"1\" width=\"100%\" border=\"1\">", ofd);
    fputs("\n  <tbody>", ofd);
    fputs("\n  <tr>", ofd);
    fputs("\n    <th nowrap=\"nowrap\" align=\"center\">", ofd);
    fputs("\n      <a href=\"../index.html\">Back to Gallery Index</a>", ofd);
    fputs("\n    </th>", ofd);
    fputs("\n  </tr>", ofd);
    fputs("\n  </tbody>", ofd);
    fputs("\n</table>", ofd);
    fputs("\n</div>", ofd);
    fputs("\n", ofd);
    fputs("\n</body>", ofd);
    fputs("\n</html>", ofd);

	fclose(ofd);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void create_gallery_photos(char * filename, int iGalleryNumber)
{
    FILE *ofd;

	if((ofd = fopen(filename, "w")) == NULL)
	{
		printf("\nFile not opened: %s\n\n", filename);
		return;
	}

	fputs("<!DOCTYPE html", ofd);
	fputs("\nPUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"", ofd);
	fputs("\n\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">", ofd);
	fputs("\n", ofd);
	fputs("\n<html xmlns=\"http://www.w3.org/1999/xhtml\">", ofd);
	fputs("\n", ofd);
	fputs("\n<head>", ofd);
	fputs("\n<title>D70Reader Slideshow</title>", ofd);
	fputs("\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1252\" />", ofd);
	fputs("\n<link href=\"../gallery.css\" type=\"text/css\" rel=\"stylesheet\" />", ofd);
	fputs("\n", ofd);
	fputs("\n<script src=\"images.js\" type=\"text/javascript\"></script>", ofd);
	fputs("\n", ofd);
	fputs("\n<script type=\"text/javascript\">", ofd);
	fputs("\n<!--", ofd);
	fputs("\nvar isIE = true;", ofd);
	fputs("\nvar intThisPic = -1;", ofd);
	fputs("\nvar charImageFilePassedIn;", ofd);
	fputs("\nvar intImageHeight = 0;", ofd);
	fputs("\nvar intImageWidth = 0;", ofd);
	fputs("\nvar intTagIndex;", ofd);
	fputs("\n", ofd);
	fputs("\nfunction removeTable() {", ofd);
	fputs("\n", ofd);
	fputs("\n    if(checkBrowser())", ofd);
	fputs("\n    {", ofd);
	fputs("\n        objectToDestroy = document.getElementById(\"_table1\");", ofd);
	fputs("\n    ", ofd);
	fputs("\n        if(objectToDestroy)", ofd);
	fputs("\n        {", ofd);
	fputs("\n            objectToDestroy.removeNode(true);", ofd);
	fputs("\n        }", ofd);
	fputs("\n    }", ofd);
	fputs("\n    else", ofd);
	fputs("\n    {    ", ofd);
	fputs("\n        var oRows = document.getElementById('_table1').getElementsByTagName('tr');", ofd);
	fputs("\n        var root = oRows[0].parentNode;", ofd);
	fputs("\n    ", ofd);
	fputs("\n        for(r=oRows.length-1;r>=0;r-=1 ) {", ofd);
	fputs("\n    ", ofd);
	fputs("\n            root.removeChild(oRows[r]);", ofd);
	fputs("\n        }", ofd);
	fputs("\n    ", ofd);
	fputs("\n        var oTBody = document.getElementById('_table1').getElementsByTagName('tbody');", ofd);
	fputs("\n        root = oTBody[0].parentNode;", ofd);
	fputs("\n        root.removeChild(oTBody[0]);", ofd);
	fputs("\n    }", ofd);
	fputs("\n}", ofd);
	fputs("\n", ofd);
	fputs("\nfunction checkBrowser() {", ofd);
	fputs("\n", ofd);
	fputs("\n    // Note: Does not work with Konqueror", ofd);
	fputs("\n", ofd);
	fputs("\n	if(navigator.appName == \"Microsoft Internet Explorer\"", ofd);
	fputs("\n		&& parseInt(navigator.appVersion) >= 4)", ofd);
	fputs("\n	{", ofd);
	fputs("\n		isIE = true;", ofd);
	fputs("\n		return true;", ofd);
	fputs("\n	}", ofd);
	fputs("\n	// For some reason, appVersion returns 5 for Netscape 6.2 ...", ofd);
	fputs("\n	else if(navigator.appName == \"Netscape\"", ofd);
	fputs("\n		&& navigator.appVersion.indexOf(\"5.\") >= 0)", ofd);
	fputs("\n	{", ofd);
	fputs("\n		isIE = false;", ofd);
	fputs("\n		return false;", ofd);
	fputs("\n	}", ofd);
	fputs("\n	else", ofd);
	fputs("\n    {", ofd);
	fputs("\n		isIE = false;", ofd);
	fputs("\n		return false;", ofd);
	fputs("\n    }", ofd);
	fputs("\n}", ofd);
	fputs("\n", ofd);
	fputs("\nfunction start() {", ofd);
	fputs("\n", ofd);
	fputs("\n        // get the reference for the body", ofd);
	fputs("\n        var mybody=document.getElementsByTagName(\"body\").item(0);", ofd);
	fputs("\n", ofd);
	fputs("\n        // creates an element whose tag name is TABLE", ofd);
	fputs("\n        mytable = document.createElement(\"TABLE\");", ofd);
	fputs("\n        mytable.style.backgroundColor=\"black\";", ofd);
	fputs("\n        mytable.id = \"_table1\";", ofd);
	fputs("\n        mytable.align = \"center\";", ofd);
	fputs("\n        mytable.border = 1;", ofd);
	fputs("\n        mytable.width = 500;", ofd);
	fputs("\n        mytable.cellpadding = 1;", ofd);
	fputs("\n", ofd);
	fputs("\n        // creates an element whose tag name is TBODY", ofd);
	fputs("\n        mytablebody = document.createElement(\"TBODY\");", ofd);
	fputs("\n", ofd);
	fputs("\n        // creating all cells", ofd);
	fputs("\n        for(j=0;j<aExifDataHeader[0].length;j++) {", ofd);
	fputs("\n            // creates an element whose tag name is TR", ofd);
	fputs("\n            mycurrent_row=document.createElement(\"TR\");", ofd);
	fputs("\n", ofd);
	fputs("\n            for(i=0;i<2;i++) {", ofd);
	fputs("\n", ofd);
	fputs("\n                // creates an element whose tag name is TD", ofd);
	fputs("\n                mycurrent_cell=document.createElement(\"TD\");", ofd);
	fputs("\n                mycurrent_cell.style.backgroundColor = \"#dddddd\";", ofd);
	fputs("\n                mycurrent_cell.style.color = \"black\";", ofd);
	fputs("\n", ofd);
	fputs("\n                if(i == 0)", ofd);
	fputs("\n                {", ofd);
	fputs("\n                    // creates a Text Node", ofd);
	fputs("\n                    currenttext=document.createTextNode(aExifDataHeader[0][j]);", ofd);
	fputs("\n                }", ofd);
	fputs("\n                else", ofd);
	fputs("\n                {", ofd);
	fputs("\n                    // creates a Text Node", ofd);
	fputs("\n                    currenttext=document.createTextNode(aExifData[intThisPic][j]);", ofd);
	fputs("\n                }", ofd);
	fputs("\n", ofd);
	fputs("\n                // appends the Text Node we created into the cell TD", ofd);
	fputs("\n                mycurrent_cell.appendChild(currenttext);", ofd);
	fputs("\n", ofd);
	fputs("\n                // appends the cell TD into the row TR", ofd);
	fputs("\n                mycurrent_row.appendChild(mycurrent_cell);", ofd);
	fputs("\n            }", ofd);
	fputs("\n", ofd);
	fputs("\n            // appends the row TR into TBODY", ofd);
	fputs("\n            mytablebody.appendChild(mycurrent_row);", ofd);
	fputs("\n        }", ofd);
	fputs("\n", ofd);
	fputs("\n        // appends TBODY into TABLE", ofd);
	fputs("\n        mytable.appendChild(mytablebody);", ofd);
	fputs("\n", ofd);
	fputs("\n        // appends TABLE into BODY", ofd);
	fputs("\n        mybody.appendChild(mytable);", ofd);
	fputs("\n}", ofd);
	fputs("\n", ofd);
	fputs("\nfunction processPrevious() {", ofd);
	fputs("\n", ofd);
	fputs("\n    if(document.images && intThisPic > 0) {", ofd);
	fputs("\n        intThisPic-=1;", ofd);
	fputs("\n", ofd);
	fputs("\n        intImageHeight = imageHeight();", ofd);
	fputs("\n        intImageWidth = imageWidth();", ofd);
	fputs("\n", ofd);
	fputs("\n        document.myPicture.src=\"images/websize/\" + aExifData[intThisPic][1];", ofd);
	fputs("\n        document.myPicture.width=intImageWidth;", ofd);
	fputs("\n        document.myPicture.height=intImageHeight;", ofd);
	fputs("\n        document.myPicture.alt = imageFile();", ofd);
	fputs("\n        removeTable();", ofd);
	fputs("\n        start();", ofd);
	fputs("\n    }", ofd);
	fputs("\n}", ofd);
	fputs("\n", ofd);
	fputs("\nfunction processNext() {", ofd);
	fputs("\n", ofd);
	fputs("\n    if(document.images && intThisPic < intExifDataArrayCount) {", ofd);
	fputs("\n        intThisPic++;", ofd);
	fputs("\n", ofd);
	fputs("\n        intImageHeight = imageHeight();", ofd);
	fputs("\n        intImageWidth = imageWidth();", ofd);
	fputs("\n", ofd);
	fputs("\n        document.myPicture.src=\"images/websize/\" + aExifData[intThisPic][1];", ofd);
	fputs("\n        document.myPicture.width=intImageWidth;", ofd);
	fputs("\n        document.myPicture.height=intImageHeight;", ofd);
	fputs("\n        document.myPicture.alt = imageFile();", ofd);
	fputs("\n        removeTable();", ofd);
	fputs("\n        start();", ofd);
	fputs("\n    }", ofd);
	fputs("\n}", ofd);
	fputs("\n", ofd);
	fputs("\nfunction imageHeight() {", ofd);
	fputs("\n", ofd);
	fputs("\n    if(intThisPic == -1)", ofd);
	fputs("\n    {", ofd);
	fputs("\n        return 1;", ofd);
	fputs("\n    }", ofd);
	fputs("\n    else", ofd);
	fputs("\n    {", ofd);
	fputs("\n        intTagIndex = getTagIndex(\"ExifImageLength\");", ofd);
	fputs("\n        return aExifData[intThisPic][intTagIndex ];", ofd);
	fputs("\n    }", ofd);
	fputs("\n}", ofd);
	fputs("\n", ofd);
	fputs("\nfunction imageWidth() {", ofd);
	fputs("\n", ofd);
	fputs("\n    if(intThisPic == -1)", ofd);
	fputs("\n    {", ofd);
	fputs("\n        return 1;", ofd);
	fputs("\n    }", ofd);
	fputs("\n    else", ofd);
	fputs("\n    {", ofd);
	fputs("\n        intTagIndex = getTagIndex(\"ExifImageWidth\");", ofd);
	fputs("\n        return aExifData[intThisPic][intTagIndex ];", ofd);
	fputs("\n    }", ofd);
	fputs("\n}", ofd);
	fputs("\n", ofd);
	fputs("\nfunction getTagIndex(taglabel) {", ofd);
	fputs("\n", ofd);
	fputs("\n    for (k=0; k<aExifDataHeader[0].length; k++)", ofd);
	fputs("\n    {", ofd);
	fputs("\n        if(aExifDataHeader[0][k] == taglabel)", ofd);
	fputs("\n        {", ofd);
	fputs("\n            return k;", ofd);
	fputs("\n        }", ofd);
	fputs("\n    }", ofd);
	fputs("\n", ofd);
	fputs("\n    return -1;", ofd);
	fputs("\n}", ofd);
	fputs("\n", ofd);
	fputs("\nfunction imageFile() {", ofd);
	fputs("\n", ofd);
	fputs("\n        if(intThisPic == -1)", ofd);
	fputs("\n            return \"blank.jpg\";", ofd);
	fputs("\n        else", ofd);
	fputs("\n            return aExifData[intThisPic][1];", ofd);
	fputs("\n}", ofd);
	fputs("\n", ofd);
	fputs("\nfunction dataElement(index) {", ofd);
	fputs("\n", ofd);
	fputs("\n    if(intThisPic == -1)", ofd);
	fputs("\n        return \"\";", ofd);
	fputs("\n    else", ofd);
	fputs("\n        return aExifData[intThisPic][index];", ofd);
	fputs("\n}", ofd);
	fputs("\n", ofd);
	fputs("\nfunction setup() {", ofd);
	fputs("\n", ofd);
	fputs("\n    for(m=0; m<=intExifDataArrayCount; m++)", ofd);
	fputs("\n    {", ofd);
	fputs("\n        if(aExifData[m][1] == charImageFilePassedIn)", ofd);
	fputs("\n        {", ofd);
	fputs("\n            intThisPic = m - 1;", ofd);
	fputs("\n", ofd);
	fputs("\n            if(document.images && intThisPic < intExifDataArrayCount) ", ofd);
	fputs("\n            {", ofd);
	fputs("\n                intThisPic++;", ofd);
	fputs("\n", ofd);
	fputs("\n                intImageHeight = imageHeight();", ofd);
	fputs("\n                intImageWidth = imageWidth();", ofd);
	fputs("\n", ofd);
	fputs("\n                document.myPicture.src=\"images/websize/\" + aExifData[intThisPic][1];", ofd);
	fputs("\n                document.myPicture.width=intImageWidth;", ofd);
	fputs("\n                document.myPicture.height=intImageHeight;", ofd);
	fputs("\n                document.myPicture.alt = imageFile();", ofd);
	fputs("\n                start();", ofd);
	fputs("\n                break;", ofd);
	fputs("\n            }", ofd);
	fputs("\n        }", ofd);
	fputs("\n    }", ofd);
	fputs("\n", ofd);
	fputs("\n}", ofd);
	fputs("\n", ofd);
	fputs("\nfunction getParm(string,parm) {", ofd);
	fputs("\n", ofd);
	fputs("\n    // returns value of parm from string", ofd);
	fputs("\n    var startPos = string.indexOf(parm + \"=\");", ofd);
	fputs("\n", ofd);
	fputs("\n    if(startPos > -1) {", ofd);
	fputs("\n        startPos = startPos + parm.length + 1;", ofd);
	fputs("\n        var endPos = string.indexOf(\"&\",startPos);", ofd);
	fputs("\n        if (endPos == -1)", ofd);
	fputs("\n            endPos = string.length;", ofd);
	fputs("\n        return unescape(string.substring(startPos,endPos));", ofd);
	fputs("\n    }", ofd);
	fputs("\n", ofd);
	fputs("\n    return '';", ofd);
	fputs("\n}", ofd);
	fputs("\n", ofd);
	fputs("\nvar passed = location.search.substring(1);", ofd);
	fputs("\n", ofd);
	fputs("\ncharImageFilePassedIn = getParm(passed,'myimage');", ofd);
	fputs("\n//-->", ofd);
	fputs("\n</script>", ofd);
	fputs("\n", ofd);
	fputs("\n</head>", ofd);
	fputs("\n<body onload='setup();'>", ofd);
	fputs("\n", ofd);
	fputs("\n<div class=\"imagepage\" id=\"imagepage\">", ofd);
	fputs("\n<div class=\"localmenu\" id=\"localmenu\">", ofd);
	fputs("\n<table border=\"0\">", ofd);
	fputs("\n  <tbody>", ofd);
	fputs("\n  <tr>", ofd);
	fputs("\n    <td>", ofd);
	fputs("\n      <h1 class=\"title\"><span class=\"title\">D70Reader Image &amp; EXIF Data</span> </h1>", ofd);
	fputs("\n    </td>", ofd);
	fputs("\n  </tr>", ofd);
	fputs("\n  </tbody>", ofd);
	fputs("\n</table>", ofd);
	fputs("\n</div>", ofd);
	fputs("\n", ofd);
	fputs("\n<div class=\"slideshow\" id=\"slideshow1\">", ofd);
	fputs("\n<table cellspacing=\"1\" width=\"100%\" border=\"0\">", ofd);
	fputs("\n  <tbody>", ofd);
	fputs("\n  <tr>", ofd);
	fputs("\n    <td nowrap=\"nowrap\" width=\"100%\">", ofd);
	fputs("\n      <a href=\"javascript:processPrevious()\">previous</a> | ", ofd);
	fputs("\n      <a href=\"javascript:processNext()\">next</a> ", ofd);
	fputs("\n    </td>", ofd);
	fputs("\n  </tr>", ofd);
	fputs("\n  </tbody>", ofd);
	fputs("\n</table>", ofd);
	fputs("\n</div>", ofd);
	fputs("\n", ofd);
	fputs("\n<!-- BEGIN image table -->", ofd);
	fputs("\n<div class=\"image\" id=\"image\">", ofd);
	fputs("\n<table class=\"imagetable\" width=\"0\" align=\"center\" border=\"0\">", ofd);
	fputs("\n  <tbody>", ofd);
	fputs("\n  <tr>", ofd);
	fputs("\n    <td colspan=\"2\">", ofd);
	fputs("\n    <a href=\"index.html\">", ofd);
	fputs("\n<script  type=\"text/javascript\">", ofd);
	fputs("\n<!--", ofd);
	fputs("\n      var myjsimage =", ofd);
	fputs("\n      \"<img class='display' alt='\" + imageFile() + \"' src='images/websize/\" + imageFile() + \"' height='\" + imageHeight() + \"' width='\" + imageWidth() + \"' border='0' NAME='myPicture'>\";", ofd);
	fputs("\n      document.write(myjsimage);", ofd);
	fputs("\n//-->", ofd);
	fputs("\n</script>", ofd);
	fputs("\n    </a>", ofd);
	fputs("\n    </td>", ofd);
	fputs("\n  </tr>", ofd);
	fputs("\n  </tbody>", ofd);
	fputs("\n</table>", ofd);
	fputs("\n</div>", ofd);
	fputs("\n<!-- END image table -->", ofd);
	fputs("\n", ofd);
	fputs("\n<div class=\"slideshow\" id=\"slideshow2\">", ofd);
	fputs("\n<table cellspacing=\"1\" width=\"100%\" border=\"0\">", ofd);
	fputs("\n  <tbody>", ofd);
	fputs("\n  <tr>", ofd);
	fputs("\n    <td nowrap=\"nowrap\" width=\"100%\">", ofd);
	fputs("\n      <a href=\"javascript:processPrevious()\">previous</a> | ", ofd);
	fputs("\n      <a href=\"javascript:processNext()\">next</a> ", ofd);
	fputs("\n    </td>", ofd);
	fputs("\n  </tr>", ofd);
	fputs("\n  </tbody>", ofd);
	fputs("\n</table>", ofd);
	fputs("\n<br></br>", ofd);
	fputs("\n</div>", ofd);
	fputs("\n</div>", ofd);
	fputs("\n</body>", ofd);
	fputs("\n</html>\n", ofd);

	fclose(ofd);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void output_web()
{
	FILE *rfd = NULL;

#ifdef WIN32
	char names[1][16];
#else
	char names[1][L_tmpnam];
#endif

	char *file_ptr = NULL;
	char *local_argv[10];

	int height = LARGE_IMAGE_HEIGHT;
	int width = LARGE_IMAGE_WIDTH;
	int iTraceLocal = 0;
	int iDoRotate = 0;
	int iHandle = 0;

	char *str_root_dir = NULL;
	char *str_root_gallery_css = NULL;
	char *str_root_index_file = NULL;
	char *str_root_image_js_file = NULL;
	char *str_gallery_image_js_file = NULL;
	char *str_gallery_dir = NULL;
	char *str_images_dir = NULL;
	char *str_thumbnail_dir = NULL;
	char *str_websize_dir = NULL;
	char *str_extraction_name = NULL;
	char *str_resize_src_name = NULL;
	char *str_resize_dst_thumbnail_name = NULL;
	char *str_resize_dst_fullsize_name = NULL;

	trace("\n+output_web");
	mtrace("\n+output_web");

	if(GlobalOrientationPtr == NULL)
	{
		if(iTrace)
		{
			printf("\nmalloc_count: %d", malloc_count);
			printf("\nfree_count: %d", free_count);
		}

		return;
	}

	if(iTrace)
		printf("\niWeb_Image_count: %d", iWeb_Image_count);

	if(iSystemType)
	{
		str_root_dir = strdup(".\\d70reader");

		str_root_image_js_file = (char *) my_malloc("str_root_image_js_file", strlen(str_root_dir) + 20);
		memset(str_root_image_js_file, (int) NULL, (strlen(str_root_dir) + 20));
		sprintf(str_root_image_js_file, "%s\\images.js", str_root_dir);

		str_root_gallery_css = (char *) my_malloc("str_root_gallery_css", strlen(str_root_dir) + 20);
		memset(str_root_gallery_css, (int) NULL, (strlen(str_root_dir) + 20));
		sprintf(str_root_gallery_css, "%s\\gallery.css", str_root_dir);

		str_root_index_file = (char *) my_malloc("str_root_index_file", strlen(str_root_dir) + 20);
		memset(str_root_index_file, (int) NULL, (strlen(str_root_dir) + 20));
		sprintf(str_root_index_file, "%s\\index.html", str_root_dir);

		str_root_temp_dir = (char *) my_malloc("str_root_temp_dir", strlen(str_root_dir) + 20);
		memset(str_root_temp_dir, (int) NULL, (strlen(str_root_dir) + 20));
		sprintf(str_root_temp_dir, "%s\\temp", str_root_dir);
	}
	else
	{
		str_root_dir = strdup("./d70reader");

		str_root_image_js_file = (char *) my_malloc("str_root_image_js_file", strlen(str_root_dir) + 20);
		memset(str_root_image_js_file, (int) NULL, (strlen(str_root_dir) + 20));
		sprintf(str_root_image_js_file, "%s/images.js", str_root_dir);

		str_root_gallery_css = (char *) my_malloc("str_root_gallery_css", strlen(str_root_dir) + 20);
		memset(str_root_gallery_css, (int) NULL, (strlen(str_root_dir) + 20));
		sprintf(str_root_gallery_css, "%s/gallery.css", str_root_dir);

		str_root_index_file = (char *) my_malloc("str_root_index_file", strlen(str_root_dir) + 20);
		memset(str_root_index_file, (int) NULL, (strlen(str_root_dir) + 20));
		sprintf(str_root_index_file, "%s/index.html", str_root_dir);

		str_root_temp_dir = (char *) my_malloc("str_root_temp_dir", strlen(str_root_dir) + 20);
		memset(str_root_temp_dir, (int) NULL, (strlen(str_root_dir) + 20));
		sprintf(str_root_temp_dir, "%s/temp", str_root_dir);
	}

	if(iWeb_First_Time)
	{
		if(my_mkdir(str_root_dir) != 0)
		{
			if(iTrace)
				printf("\nFailed to create '%s' directory", str_root_dir);
		}

		if(my_mkdir(str_root_temp_dir) != 0)
		{
			if(iTrace)
				printf("\nFailed to create '%s' directory", str_root_temp_dir);
		}

		create_root_index(str_root_index_file);
		create_css(str_root_gallery_css);
		create_gallery(str_root_dir);

		if(iSystemType)
		{
			str_gallery_image_js_file = (char *) my_malloc("str_gallery_image_js_file", strlen(str_root_dir) + 30);
			memset(str_gallery_image_js_file, (int) NULL, (strlen(str_root_dir) + 30));
			sprintf(str_gallery_image_js_file, "%s\\gallery%d\\images.js", str_root_dir, iWeb_Gallery_count);
		}
		else
		{
			str_gallery_image_js_file = (char *) my_malloc("str_gallery_image_js_file", strlen(str_root_dir) + 30);
			memset(str_gallery_image_js_file, (int) NULL, (strlen(str_root_dir) + 30));
			sprintf(str_gallery_image_js_file, "%s/gallery%d/images.js", str_root_dir, iWeb_Gallery_count);
		}

		if((root_ofd = fopen(str_root_image_js_file, "w")) == NULL)
		{
			if(iTrace)
				printf("\nFile not opened: %s\n\n", str_root_image_js_file);

			return;
		}

		output_js(root_ofd, 1, iRoot_Web_Image_count++);

		if((gallery_ofd = fopen(str_gallery_image_js_file, "w")) == NULL)
		{
			if(iTrace)
				printf("\nFile not opened: %s\n\n", str_gallery_image_js_file);

			return;
		}

		iWeb_First_Time = 0;
	}

	if(iWeb_Image_count == 24)
	{
		fprintf(gallery_ofd, "var intExifDataArrayCount=%d\n", (iWeb_Image_count - 1));

		iJS_First_Time = 1;
		iWeb_Image_count = 0;
		create_gallery(str_root_dir);

		fclose(gallery_ofd);

		if(str_gallery_image_js_file != NULL)
			my_free("str_gallery_image_js_file", str_gallery_image_js_file);

		if(iSystemType)
		{
			str_gallery_image_js_file = (char *) my_malloc("str_gallery_image_js_file", strlen(str_root_dir) + 30);
			memset(str_gallery_image_js_file, (int) NULL, (strlen(str_root_dir) + 30));
			sprintf(str_gallery_image_js_file, "%s\\gallery%d\\images.js", str_root_dir, iWeb_Gallery_count);
		}
		else
		{
			str_gallery_image_js_file = (char *) my_malloc("str_gallery_image_js_file", strlen(str_root_dir) + 30);
			memset(str_gallery_image_js_file, (int) NULL, (strlen(str_root_dir) + 30));
			sprintf(str_gallery_image_js_file, "%s/gallery%d/images.js", str_root_dir, iWeb_Gallery_count);
		}

		if((gallery_ofd = fopen(str_gallery_image_js_file, "w")) == NULL)
		{
			printf("\nFile not opened: %s\n\n", str_gallery_image_js_file);
			return;
		}

		output_js(root_ofd, 0, iRoot_Web_Image_count++);
	}

	if(iSystemType)
	{
		str_gallery_dir = (char *) my_malloc("str_gallery_dir", strlen(str_root_dir) + 20);
		memset(str_gallery_dir, (int) NULL, (strlen(str_root_dir) + 20));
		sprintf(str_gallery_dir, "%s\\gallery%d", str_root_dir, iWeb_Gallery_count);
		
		str_images_dir = (char *) my_malloc("str_images_dir", strlen(str_gallery_dir) + 10);
		memset(str_images_dir, (int) NULL, (strlen(str_gallery_dir) + 10));
		sprintf(str_images_dir, "%s\\images", str_gallery_dir);

		str_thumbnail_dir = (char *) my_malloc("str_thumbnail_dir", strlen(str_images_dir) + 20);
		memset(str_thumbnail_dir, (int) NULL, (strlen(str_images_dir) + 20));
		sprintf(str_thumbnail_dir, "%s\\thumbnail", str_images_dir);

		str_websize_dir = (char *) my_malloc("str_websize_dir", strlen(str_images_dir) + 10);
		memset(str_websize_dir, (int) NULL, (strlen(str_images_dir) + 10));
		sprintf(str_websize_dir, "%s\\websize", str_images_dir);

	}
	else
	{
		str_gallery_dir = (char *) my_malloc("str_gallery_dir", strlen(str_root_dir) + 20);
		memset(str_gallery_dir, (int) NULL, (strlen(str_root_dir) + 20));
		sprintf(str_gallery_dir, "%s/gallery%d", str_root_dir, iWeb_Gallery_count);
		
		str_images_dir = (char *) my_malloc("str_images_dir", strlen(str_gallery_dir) + 10);
		memset(str_images_dir, (int) NULL, (strlen(str_gallery_dir) + 10));
		sprintf(str_images_dir, "%s/images", str_gallery_dir);

		str_thumbnail_dir = (char *) my_malloc("str_thumbnail_dir", strlen(str_images_dir) + 20);
		memset(str_thumbnail_dir, (int) NULL, (strlen(str_images_dir) + 20));
		sprintf(str_thumbnail_dir, "%s/thumbnail", str_images_dir);

		str_websize_dir = (char *) my_malloc("str_websize_dir", strlen(str_images_dir) + 10);
		memset(str_websize_dir, (int) NULL, (strlen(str_images_dir) + 10));
		sprintf(str_websize_dir, "%s/websize", str_images_dir);

	}

	if(iJS_First_Time)
	{
		iJS_First_Time = 0;
		output_js(gallery_ofd, 1, iWeb_Image_count);
	}
	else
		output_js(gallery_ofd, 0, iWeb_Image_count);


	iWeb_Image_count++;

	if(iTrace)
		printf("\nGlobalOrientationPtr: %s", GlobalOrientationPtr);

	if((iGlobalImageHeight != 0) && (iGlobalImageWidth != 0))
	{
		if((!strcmp(GlobalOrientationPtr, "6") || !strcmp(GlobalOrientationPtr, "8")) || 
					(!strcmp(GlobalOrientationPtr, "1") && (iGlobalImageHeight > iGlobalImageWidth)) ||
					(!strcmp(GlobalOrientationPtr, "X") && (iGlobalImageHeight > iGlobalImageWidth)))
		{
			if(iTrace)
				printf("\noutput_web - vertical");

			height = LARGE_IMAGE_LENGTH;

			if(!strcmp(GlobalOrientationPtr, "1") || !strcmp(GlobalOrientationPtr, "X"))
				width = (height * iGlobalImageWidth) / iGlobalImageHeight;
			else
			{
				if(iGlobalImageHeight > iGlobalImageWidth)
					width = (height * iGlobalImageWidth) / iGlobalImageHeight;
				else
					width = (height * iGlobalImageHeight) / iGlobalImageWidth;
			}
		}
		else
		{
			if(iTrace)
				printf("\noutput_web - horizontal");

			width = LARGE_IMAGE_LENGTH;
			height = (width * iGlobalImageHeight) / iGlobalImageWidth;
		}
	}

	if(iTrace)
	{
		printf("\nw_width: %d", width);
		printf("\nw_height: %d", height);
	}

	if(iFile_Type)
	{
		if((rfd = fopen(web_file_name_src_ptr, "rb")) == NULL)
		{
			printf("\nFile not opened: %s\n\n", web_file_name_src_ptr);
			return;
		}

		if(iTraceLocal)
		{
			printf("\nstr_root_temp_dir: %s", str_root_temp_dir);
			printf("\nweb_file_name_src_ptr: %s", web_file_name_src_ptr);
			printf("\nweb_file_name_tmp_ptr: %s", web_file_name_tmp_ptr);

			printf("\nstr_gallery_dir: %s", str_gallery_dir);
			printf("\nstr_images_dir: %s", str_images_dir);
			printf("\nstr_thumbnail_dir: %s", str_thumbnail_dir);
			printf("\nstr_websize_dir: %s", str_websize_dir);
		}
/*

str_root_temp_dir: .\d70reader\temp
web_file_name_src_ptr: DSC_0003.NEF
web_file_name_tmp_ptr: DSC_0003.NEF.JPG
str_gallery_dir: .\d70reader\gallery1
str_images_dir: .\d70reader\gallery1\images
str_thumbnail_dir: .\d70reader\gallery1\images\thumbnail
str_websize_dir: .\d70reader\gallery1\images\websize
str_extraction_name: .\d70reader\temp\DSC_0003.NEF
str_resize_src_name: .\d70reader\temp\DSC_0003.NEF.JPG
str_resize_dst_thumbnail_name: .\d70reader\gallery1\images\thumbnail\DSC_0003.NEF.JPG
str_resize_dst_fullsize_name: .\d70reader\gallery1\images\websize\DSC_0003.NEF.JPG

*/

		if(iSystemType)
		{
			str_extraction_name = (char *) my_malloc("str_extraction_name", strlen(str_root_temp_dir) + strlen(web_file_name_src_ptr) + 20);
			memset(str_extraction_name, (int) NULL, (strlen(str_root_temp_dir) + strlen(web_file_name_src_ptr) + 20));
			sprintf(str_extraction_name, "%s\\%s", str_root_temp_dir, web_file_name_src_ptr);
		}
		else
		{
			str_extraction_name = (char *) my_malloc("str_extraction_name", strlen(str_root_temp_dir) + strlen(web_file_name_src_ptr) + 20);
			memset(str_extraction_name, (int) NULL, (strlen(str_root_temp_dir) + strlen(web_file_name_src_ptr) + 20));
			sprintf(str_extraction_name, "%s/%s", str_root_temp_dir, web_file_name_src_ptr);
		}

		if(iTraceLocal)
			printf("\nstr_extraction_name: %s", str_extraction_name);

		extract_thumbnail(str_extraction_name, rfd);

		if(iSystemType)
		{
			str_resize_src_name = (char *) my_malloc("str_resize_src_name", strlen(str_root_temp_dir) + strlen(web_file_name_tmp_ptr) + 20);
			memset(str_resize_src_name, (int) NULL, (strlen(str_root_temp_dir) + strlen(web_file_name_tmp_ptr) + 20));
			sprintf(str_resize_src_name, "%s\\%s", str_root_temp_dir, web_file_name_tmp_ptr);

			str_resize_dst_thumbnail_name = (char *) my_malloc("str_resize_dst_thumbnail_name", strlen(str_thumbnail_dir) + strlen(web_file_name_tmp_ptr) + 20);
			memset(str_resize_dst_thumbnail_name, (int) NULL, (strlen(str_thumbnail_dir) + strlen(web_file_name_tmp_ptr) + 20));
			sprintf(str_resize_dst_thumbnail_name, "%s\\%s", str_thumbnail_dir, web_file_name_tmp_ptr);

			str_resize_dst_fullsize_name = (char *) my_malloc("str_resize_dst_fullsize_name", strlen(str_websize_dir) + strlen(web_file_name_tmp_ptr) + 20);
			memset(str_resize_dst_fullsize_name, (int) NULL, (strlen(str_websize_dir) + strlen(web_file_name_tmp_ptr) + 20));
			sprintf(str_resize_dst_fullsize_name, "%s\\%s", str_websize_dir, web_file_name_tmp_ptr);
		}
		else
		{
			str_resize_src_name = (char *) my_malloc("str_resize_src_name", strlen(str_root_temp_dir) + strlen(web_file_name_tmp_ptr) + 20);
			memset(str_resize_src_name, (int) NULL, (strlen(str_root_temp_dir) + strlen(web_file_name_tmp_ptr) + 20));
			sprintf(str_resize_src_name, "%s/%s", str_root_temp_dir, web_file_name_tmp_ptr);

			str_resize_dst_thumbnail_name = (char *) my_malloc("str_resize_dst_thumbnail_name", strlen(str_thumbnail_dir) + strlen(web_file_name_tmp_ptr) + 20);
			memset(str_resize_dst_thumbnail_name, (int) NULL, (strlen(str_thumbnail_dir) + strlen(web_file_name_tmp_ptr) + 20));
			sprintf(str_resize_dst_thumbnail_name, "%s/%s", str_thumbnail_dir, web_file_name_tmp_ptr);

			str_resize_dst_fullsize_name = (char *) my_malloc("str_resize_dst_fullsize_name", strlen(str_websize_dir) + strlen(web_file_name_tmp_ptr) + 20);
			memset(str_resize_dst_fullsize_name, (int) NULL, (strlen(str_websize_dir) + strlen(web_file_name_tmp_ptr) + 20));
			sprintf(str_resize_dst_fullsize_name, "%s/%s", str_websize_dir, web_file_name_tmp_ptr);
		}

		if(iTraceLocal)
		{
			printf("\nstr_resize_src_name: %s", str_resize_src_name);
			printf("\nstr_resize_dst_thumbnail_name: %s", str_resize_dst_thumbnail_name);
			printf("\nstr_resize_dst_fullsize_name: %s", str_resize_dst_fullsize_name);
		}

		resize(str_resize_src_name, str_resize_dst_thumbnail_name, (width/4), 0, 75);
		resize(str_resize_src_name, str_resize_dst_fullsize_name, width, 0, 90);

		fclose(rfd);

		unlink(str_resize_src_name);
	}
	else
	{
		if(iSystemType)
		{
			str_resize_src_name = (char *) my_malloc("str_resize_src_name", strlen(str_root_temp_dir) + strlen(web_file_name_tmp_ptr) + 20);
			memset(str_resize_src_name, (int) NULL, (strlen(str_root_temp_dir) + strlen(web_file_name_tmp_ptr) + 20));
			sprintf(str_resize_src_name, ".\\%s", web_file_name_tmp_ptr);

			str_resize_dst_thumbnail_name = (char *) my_malloc("str_resize_dst_thumbnail_name", strlen(str_thumbnail_dir) + strlen(web_file_name_tmp_ptr) + 20);
			memset(str_resize_dst_thumbnail_name, (int) NULL, (strlen(str_thumbnail_dir) + strlen(web_file_name_tmp_ptr) + 20));
			sprintf(str_resize_dst_thumbnail_name, "%s\\%s", str_thumbnail_dir, web_file_name_tmp_ptr);

			str_resize_dst_fullsize_name = (char *) my_malloc("str_resize_dst_fullsize_name", strlen(str_websize_dir) + strlen(web_file_name_tmp_ptr) + 20);
			memset(str_resize_dst_fullsize_name, (int) NULL, (strlen(str_websize_dir) + strlen(web_file_name_tmp_ptr) + 20));
			sprintf(str_resize_dst_fullsize_name, "%s\\%s", str_websize_dir, web_file_name_tmp_ptr);
		}
		else
		{
			str_resize_src_name = (char *) my_malloc("str_resize_src_name", strlen(str_root_temp_dir) + strlen(web_file_name_tmp_ptr) + 20);
			memset(str_resize_src_name, (int) NULL, (strlen(str_root_temp_dir) + strlen(web_file_name_tmp_ptr) + 20));
			sprintf(str_resize_src_name, "./%s", web_file_name_tmp_ptr);

			str_resize_dst_thumbnail_name = (char *) my_malloc("str_resize_dst_thumbnail_name", strlen(str_thumbnail_dir) + strlen(web_file_name_tmp_ptr) + 20);
			memset(str_resize_dst_thumbnail_name, (int) NULL, (strlen(str_thumbnail_dir) + strlen(web_file_name_tmp_ptr) + 20));
			sprintf(str_resize_dst_thumbnail_name, "%s/%s", str_thumbnail_dir, web_file_name_tmp_ptr);

			str_resize_dst_fullsize_name = (char *) my_malloc("str_resize_dst_fullsize_name", strlen(str_websize_dir) + strlen(web_file_name_tmp_ptr) + 20);
			memset(str_resize_dst_fullsize_name, (int) NULL, (strlen(str_websize_dir) + strlen(web_file_name_tmp_ptr) + 20));
			sprintf(str_resize_dst_fullsize_name, "%s/%s", str_websize_dir, web_file_name_tmp_ptr);
		}

		if(iTraceLocal)
		{
			printf("\nstr_root_dir: %s", str_root_dir);
			printf("\nstr_root_temp_dir: %s", str_root_temp_dir);
			printf("\nstr_root_gallery_css: %s", str_root_gallery_css);
			printf("\nstr_root_index_file: %s", str_root_index_file);
			printf("\nstr_root_image_js_file: %s", str_root_image_js_file);
			printf("\nstr_gallery_image_js_file: %s", str_gallery_image_js_file);
			printf("\nstr_gallery_dir: %s", str_gallery_dir);
			printf("\nstr_images_dir: %s", str_images_dir);
			printf("\nstr_thumbnail_dir: %s", str_thumbnail_dir);
			printf("\nstr_websize_dir: %s", str_websize_dir);
			printf("\nstr_extraction_name: %s", str_extraction_name);
			printf("\nstr_resize_src_name: %s", str_resize_src_name);
			printf("\nstr_resize_dst_thumbnail_name: %s", str_resize_dst_thumbnail_name);
			printf("\nstr_resize_dst_fullsize_name: %s", str_resize_dst_fullsize_name);
		}

		if(iDoRotate)
		{
#ifdef WIN32
			strcpy( names[0], "d70XXXXXX" );
			file_ptr = _mktemp(names[0]);
#else
//			file_ptr = tempnam("", "d70");

			strcpy( names[0], "d70XXXXXX" );

			if((iHandle = mkstemp(names[0])) == -1)
			{
				my_free("str_root_gallery_css", str_root_gallery_css);
				my_free("str_root_index_file", str_root_index_file);
				my_free("str_root_image_js_file", str_root_image_js_file);
			
				if(str_gallery_image_js_file != NULL)
				{
					my_free("str_gallery_image_js_file", str_gallery_image_js_file);
					str_gallery_image_js_file = NULL;
				}
			
				my_free("str_gallery_dir", str_gallery_dir);
				my_free("str_images_dir", str_images_dir);
				my_free("str_thumbnail_dir", str_thumbnail_dir);
				my_free("str_websize_dir", str_websize_dir);
			
				if(iFile_Type)
					my_free("str_extraction_name", str_extraction_name);
			
				my_free("str_resize_src_name", str_resize_src_name);
				my_free("str_resize_dst_thumbnail_name", str_resize_dst_thumbnail_name);
				my_free("str_resize_dst_fullsize_name", str_resize_dst_fullsize_name);
			
				iGlobalImageWidth = 0;
				iGlobalImageHeight = 0;
				iSizeTestCompleted = 0;

				if(GlobalOrientationPtr != NULL)
				{
					my_free("GlobalOrientationPtr", GlobalOrientationPtr);
					GlobalOrientationPtr = NULL;
				}

				printf("\nCreation of 'temp' file name failed\n");
				return;
			}

			file_ptr = names[0];
			close(iHandle);
#endif

			local_argv[0] = "jpgtran";
			local_argv[1] = "-rot";
	
			if(!strcmp(GlobalOrientationPtr, "6"))
				local_argv[2] = "90";
			else
				local_argv[2] = "270";
	
			local_argv[3] = "-copy";
			local_argv[4] = "all";
			local_argv[5] = str_resize_src_name;
			local_argv[6] = file_ptr;
			local_argv[7] = NULL;

			jpegtran(7, local_argv);

		}

		if(iDoRotate)
		{
			resize(file_ptr, str_resize_dst_thumbnail_name, (width/4), 0, 75);
			resize(file_ptr, str_resize_dst_fullsize_name, width, 0, 90);

			if(remove(file_ptr))
				printf("File not deleted: %s\n", file_ptr);
		}
		else
		{
			resize(str_resize_src_name, str_resize_dst_thumbnail_name, (width/4), 0, 75);
			resize(str_resize_src_name, str_resize_dst_fullsize_name, width, 0, 90);
		}
	}

	if(GlobalOrientationPtr != NULL)
	{
		my_free("GlobalOrientationPtr", GlobalOrientationPtr);
		GlobalOrientationPtr = NULL;
	}

	//my_free("str_root_dir", str_root_dir);  // don't do this!  used strdup for this one!
	my_free("str_root_gallery_css", str_root_gallery_css);
	my_free("str_root_index_file", str_root_index_file);
	my_free("str_root_image_js_file", str_root_image_js_file);

	if(str_gallery_image_js_file != NULL)
	{
		my_free("str_gallery_image_js_file", str_gallery_image_js_file);
		str_gallery_image_js_file = NULL;
	}

	my_free("str_gallery_dir", str_gallery_dir);
	my_free("str_images_dir", str_images_dir);
	my_free("str_thumbnail_dir", str_thumbnail_dir);
	my_free("str_websize_dir", str_websize_dir);

	if(iFile_Type)
		my_free("str_extraction_name", str_extraction_name);

	my_free("str_resize_src_name", str_resize_src_name);
	my_free("str_resize_dst_thumbnail_name", str_resize_dst_thumbnail_name);
	my_free("str_resize_dst_fullsize_name", str_resize_dst_fullsize_name);

	iGlobalImageWidth = 0;
	iGlobalImageHeight = 0;
	iSizeTestCompleted = 0;

	if(iTrace)
	{
		printf("\nmalloc_count: %d", malloc_count);
		printf("\nfree_count: %d", free_count);
	}

	trace("\n-output_web");
	mtrace("\n-output_web");
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void create_gallery(char *str_ptr)
{
	char *str_gallery_index_file = NULL;
	char *str_gallery_photos_file = NULL;
	char *str_gallery_dir = NULL;
	char *str_images_dir = NULL;
	char *str_thumbnail_dir = NULL;
	char *str_websize_dir = NULL;

	trace("\n+create_gallery");
	mtrace("\n+create_gallery");

	iWeb_Gallery_count++;

	if(iSystemType)
	{
		str_gallery_dir = (char *) my_malloc("str_gallery_dir", strlen(str_ptr) + 20);
		memset(str_gallery_dir, (int) NULL, (strlen(str_ptr) + 20));
		sprintf(str_gallery_dir, "%s\\gallery%d", str_ptr, iWeb_Gallery_count);

		str_images_dir = (char *) my_malloc("str_images_dir", strlen(str_gallery_dir) + 10);
		memset(str_images_dir, (int) NULL, (strlen(str_gallery_dir) + 10));
		sprintf(str_images_dir, "%s\\images", str_gallery_dir);

		str_thumbnail_dir = (char *) my_malloc("str_thumbnail_dir", strlen(str_images_dir) + 20);
		memset(str_thumbnail_dir, (int) NULL, (strlen(str_images_dir) + 20));
		sprintf(str_thumbnail_dir, "%s\\thumbnail", str_images_dir);

		str_websize_dir = (char *) my_malloc("str_websize_dir", strlen(str_images_dir) + 10);
		memset(str_websize_dir, (int) NULL, (strlen(str_images_dir) + 10));
		sprintf(str_websize_dir, "%s\\websize", str_images_dir);
	}
	else
	{
		str_gallery_dir = (char *) my_malloc("str_gallery_dir", strlen(str_ptr) + 20);
		memset(str_gallery_dir, (int) NULL, (strlen(str_ptr) + 20));
		sprintf(str_gallery_dir, "%s/gallery%d", str_ptr, iWeb_Gallery_count);

		str_images_dir = (char *) my_malloc("str_images_dir", strlen(str_gallery_dir) + 10);
		memset(str_images_dir, (int) NULL, (strlen(str_gallery_dir) + 10));
		sprintf(str_images_dir, "%s/images", str_gallery_dir);

		str_thumbnail_dir = (char *) my_malloc("str_thumbnail_dir", strlen(str_images_dir) + 20);
		memset(str_thumbnail_dir, (int) NULL, (strlen(str_images_dir) + 20));
		sprintf(str_thumbnail_dir, "%s/thumbnail", str_images_dir);

		str_websize_dir = (char *) my_malloc("str_websize_dir", strlen(str_images_dir) + 10);
		memset(str_websize_dir, (int) NULL, (strlen(str_images_dir) + 10));
		sprintf(str_websize_dir, "%s/websize", str_images_dir);
	}

	if(my_mkdir(str_gallery_dir) != 0)
	{
		if(iTrace)
			printf("\nFailed to create '%s' directory", str_gallery_dir);
	}

	if(my_mkdir(str_images_dir) != 0)
	{
		if(iTrace)
			printf("\nFailed to create '%s' directory", str_images_dir);
	}

	if(my_mkdir(str_thumbnail_dir) != 0)
	{
		if(iTrace)
			printf("\nFailed to create '%s' directory", str_thumbnail_dir);
	}

	if(my_mkdir(str_websize_dir) != 0)
	{
		if(iTrace)
			printf("\nFailed to create '%s' directory", str_websize_dir);
	}

	if(iSystemType)
	{
		str_gallery_photos_file = (char *) my_malloc("str_gallery_photos_file", strlen(str_gallery_dir) + 20);
		memset(str_gallery_photos_file, (int) NULL, (strlen(str_gallery_dir) + 20));
		sprintf(str_gallery_photos_file, "%s\\photos.html", str_gallery_dir);

		str_gallery_index_file = (char *) my_malloc("str_gallery_index_file", strlen(str_gallery_dir) + 20);
		memset(str_gallery_index_file, (int) NULL, (strlen(str_gallery_dir) + 20));
		sprintf(str_gallery_index_file, "%s\\index.html", str_gallery_dir);
	}
	else
	{
		str_gallery_photos_file = (char *) my_malloc("str_gallery_photos_file", strlen(str_gallery_dir) + 20);
		memset(str_gallery_photos_file, (int) NULL, (strlen(str_gallery_dir) + 20));
		sprintf(str_gallery_photos_file, "%s/photos.html", str_gallery_dir);

		str_gallery_index_file = (char *) my_malloc("str_gallery_index_file", strlen(str_gallery_dir) + 20);
		memset(str_gallery_index_file, (int) NULL, (strlen(str_gallery_dir) + 20));
		sprintf(str_gallery_index_file, "%s/index.html", str_gallery_dir);
	}

	create_gallery_photos(str_gallery_photos_file, iWeb_Gallery_count);

	create_gallery_index(str_gallery_index_file, iWeb_Gallery_count);

	if(str_gallery_index_file != NULL)
		my_free("str_gallery_index_file", str_gallery_index_file);

	if(str_gallery_photos_file != NULL)
		my_free("str_gallery_photos_file", str_gallery_photos_file);
	
	if(str_gallery_dir != NULL)
		my_free("str_gallery_dir", str_gallery_dir);

	if(str_images_dir != NULL)
		my_free("str_images_dir", str_images_dir);

	if(str_thumbnail_dir != NULL)
		my_free("str_thumbnail_dir", str_thumbnail_dir);

	if(str_websize_dir != NULL)
		my_free("str_websize_dir", str_websize_dir);

	trace("\n-create_gallery");
	mtrace("\n-create_gallery");
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void extract_thumbnail(char *filename, FILE *rfd)
{
	FILE *ofd = NULL;

	char *file_tmp_ptr = NULL;
	unsigned char *file_ptr = NULL;
	unsigned char *byte_ptr = NULL;
	unsigned long size;
	char *local_argv[10];

	trace("\n+extract_thumbnail");
	mtrace("\n+extract_thumbnail");

	if(ALLOW_ROTATE)
	{
		if(!strcmp(GlobalOrientationPtr, "6") || !strcmp(GlobalOrientationPtr, "8"))
		{
			file_tmp_ptr = (char *) my_malloc("file_tmp_ptr", strlen(filename) + strlen(".tmp") + strlen(".JPG") + 1);
			memset(file_tmp_ptr, (int) NULL, (strlen(filename) + strlen(".tmp") + strlen(".JPG") + 1));
			strcat(file_tmp_ptr, filename);
			strcat(file_tmp_ptr, ".tmp.JPG");

			if(iTrace)
				printf("\nfile_tmp_ptr: %s\n", file_tmp_ptr);
		}
	}

	file_ptr = (char *) my_malloc("file_ptr", strlen(filename) + strlen(".JPG") + 1);
	memset(file_ptr, (int) NULL, (strlen(filename) + strlen(".JPG") + 1));
	strcat(file_ptr, filename);
	strcat(file_ptr, ".JPG");

	if(iTrace)
		printf("\nfile_ptr: %s\n", file_ptr);

	byte_ptr = (char *) my_malloc("byte_ptr", lThumbnailLength);

	if(!strncmp(upper_case(model_ptr), "NIKON D100", strlen("NIKON D100")) ||
	   !strncmp(upper_case(model_ptr), "NIKON D2H", strlen("NIKON D2H")))
	{
		fseek(rfd, lThumbnailOffset + calc_maker_note_header_offset, 0);
	}
	else
	{
		fseek(rfd, lThumbnailOffset, 0);
	}

	fread(byte_ptr, lThumbnailLength, 1, rfd);

	if(!strcmp(GlobalOrientationPtr, "6") || !strcmp(GlobalOrientationPtr, "8"))
	{
		if(ALLOW_ROTATE)
		{
			if((ofd = fopen(file_tmp_ptr, "wb")) == NULL)
			{
				printf("\nFile not created: %s\n\n", file_tmp_ptr);
				exit(1);
			}
		}
		else
		{
			if((ofd = fopen(file_ptr, "wb")) == NULL)
			{
				printf("\nFile not created: %s\n\n", file_ptr);
				exit(1);
			}
		}
	}
	else
	{
		if((ofd = fopen(file_ptr, "wb")) == NULL)
		{
			printf("\nFile not created: %s\n\n", file_ptr);
			exit(1);
		}
	}

	if((size = fwrite(byte_ptr, lThumbnailLength, 1, ofd)) == 0)
	{
		printf("\nFile not written: %s\n\n", file_ptr);
		exit(1);
	}

	fclose(ofd);

	if(ALLOW_ROTATE)
	{
		if(!strcmp(GlobalOrientationPtr, "6") || !strcmp(GlobalOrientationPtr, "8"))
		{
			local_argv[0] = "jpgtran";
			local_argv[1] = "-rot";
	
			if(!strcmp(GlobalOrientationPtr, "6"))
				local_argv[2] = "90";
			else
				local_argv[2] = "270";
	
			local_argv[3] = "-copy";
			local_argv[4] = "all";
			local_argv[5] = file_tmp_ptr;
			local_argv[6] = file_ptr;
			local_argv[7] = NULL;

			jpegtran(7, local_argv);

			if(remove(file_tmp_ptr))
				printf("File not deleted: %s\n", file_tmp_ptr);
		}

	}

	my_free("byte_ptr", byte_ptr);
	my_free("file_ptr", file_ptr);

	if(file_tmp_ptr != NULL)
		my_free("file_tmp_ptr", file_tmp_ptr);

	trace("\n-extract_thumbnail");
	mtrace("\n-extract_thumbnail");
}

