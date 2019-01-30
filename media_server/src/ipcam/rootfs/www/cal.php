<?php
$language=array();
include "lib.php";
is_login();
$admin = $_SESSION["admin"];
check_ramdisk("umount");
load_lang();

$form1 = trim($_REQUEST["Form"]);
$DateControl = trim($_REQUEST["DateControl"]);


$body_color = "#EEEEEE";
$input_css = "color:#FFFFFF;font-family:Trebuchet MS, Verdana, Arial, Helvetica, sans-serif;font-size:12px;border-collapse: collapse; background:#8E3748;border-width:1px;border-color:#8E3748; LINE-HEIGHT: 12px;width:30 px;";
$top_css = "color:#FFFFFF;font-family:Trebuchet MS, Verdana, Arial, Helvetica, sans-serif;font-size:14px;border-collapse: collapse; background:#8E3748;border-width:1px;border-color:#8E3748; LINE-HEIGHT: 12px;width:30 px;";
$top_css1 = "color:#537A87;font-family:Trebuchet MS, Verdana, Arial, Helvetica, sans-serif;font-size:14px;border-collapse: collapse; background:#FFFFFF;border-width:1px;border-color:#8E3748; LINE-HEIGHT: 12px;width:30 px;";


?>

<HTML>
<HEAD>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>NVR <?=$language["Calendar"]?></title>
<script type="text/javascript" src="scripts/initial_nv.js"></script>
<link rel="stylesheet" type="text/css" href="scripts/ping.css" />
<STYLE TYPE="text/css">
<!-- hide from CSS-challenged browsers
	A				{font-family:Verdana,Arial,Helvetica,Sans-Serif;font-size:12px;}
	A:link			{font-family:Verdana,Arial,Helvetica,Sans-Serif;font-size:12px;color:blue;}
	A:visited		{font-family:Verdana,Arial,Helvetica,Sans-Serif;font-size:12px;color:blue;}
	A:active		{font-family:Verdana,Arial,Helvetica,Sans-Serif;font-size:12px;color:red;}
	A:hover			{font-family:Verdana,Arial,Helvetica,Sans-Serif;font-size:12px;color:red;}
	H1				{font-size:18px;margin-bottom:10px;}
	BODY,
	TD				{font-family:Verdana,Arial,Helvetica,Sans-Serif;color:#8E3748;font-size:12px;}
	TD.small		{font-family:Verdana,Arial,Helvetica,Sans-Serif;color:#333333;font-size:14px;}
	STRONG			{font-weight:bold;font-style:normal;}
	INPUT			{font-family:none; background-color: #8E3748;color:#FFFFFF;}
	INPUT.small		{font-family:none;font-size:12px;}
	SELECT.small	{font-family:none;font-size:12px;}
	H2.head			{margin: 1em 2em;color: #003333;font-size: 14pt;font-family: sans-serif;font-weight: bold;}
	H2 SMALL		{color: #003333;font-size: 70%;}
	H2 HR			{margin: 0em 0em;color: #003333;}
	.cnt {font-family:Verdana;font-size:12px;}
	
	
.submitBtn1 {

	border: solid 1px #333333;
	height: 24px;
	width: 26px;
	font-family: Trebuchet MS, Verdana, Arial, Helvetica, sans-serif;
	font-size: 10px;
	font-weight: bold;
	color: #FFFFFF;
	cursor: hand;
}
.submitBtn2 {

	border: solid 1px #333333;
	height: 24px;
	width: 26px;
	font-family: Trebuchet MS, Verdana, Arial, Helvetica, sans-serif;
	font-size: 12px;
	font-weight: bold;
	color: #FFFFFF;
}


// done hiding -->
</STYLE>
<SCRIPT LANGUAGE="JavaScript">

myDate = new Date();
this.day = myDate.getDate();
this.weekday = myDate.getDay()+1;
this.month = myDate.getMonth();
this.year = myDate.getFullYear();


this.focusday = this.day;
userFormat = 'yyyy/mm/dd';
function BrowserInfo() {
	this.appVersion = parseInt(navigator.appVersion);
	this.appName = navigator.appName;
	this.browserCode = '';
	this.isMS = false;
	this.isWindows = ((navigator.userAgent.indexOf("Win") != -1) ? true : false)
	if (this.appName == 'Netscape') {
		this.browserCode = 'ns' + this.appVersion;
	}
	else if (this.appName == 'Microsoft Internet Explorer') {
		this.isMS = true;
		if (this.appVersion >= 4)
			this.browserCode = 'ie' + this.appVersion;
		else
			this.browserCode = 'ie3';
	}
}
var gBrowserInfo = new BrowserInfo();
</SCRIPT>

<SCRIPT LANGUAGE="JavaScript">

function setDate() {

    this.dateField   = opener.document.<?=$form1?>.<?=$DateControl?>;
    this.inDate      = dateField.value;
		//if (this.inDate == '')	this.inDate =	new Date();

    // SET DAY MONTH AND YEAR TO TODAY'S DATE
    var day   = this.day;
    var month = this.month;
    var year  = this.year;
    if (year < 100)
        year += 1900;

    // IF A DATE WAS PASSED IN THEN PARSE THAT DATE
    
    if (inDate.length > 0) {

       var inDay=inDate.substring(inDate.lastIndexOf('/')+1, inDate.length);

       if (inDay.substring(0,1) == "0" && inDay.length > 1)
                inDay = inDay.substring(1,inDay.length);
            inDay = parseInt(inDay);
       var inMonth = inDate.substring(inDate.indexOf('/') + 1, inDate.lastIndexOf('/'));
       
       if (userFormat == 'dd/mmm/yyyy')
	   {
	     
	     var monthControl = document.calControl.month;
	     for (var i= 0; i< monthControl.options.length; i++)
	     {
	       if ((monthControl.options[i].text).indexOf(inMonth) != -1)
		     inMonth = (i + 1) + "";
	     }
       }
		
	   if (inMonth.substring(0,1) == "0" && inMonth.length > 1)
            inMonth = inMonth.substring(1,inMonth.length);
            inMonth = parseInt(inMonth);
        
        var inYear = inDate.substring(0, inDate.indexOf('/'));
        day = inDay;
        month = inMonth-1;
        year = inYear;
    }
    
    this.focusDay                           = day;
    document.calControl.month.selectedIndex = month;
    document.calControl.year.value          = year;
    displayCalendar(day, month, year);
}


function setToday() {
    // SET DAY MONTH AND YEAR TO TODAY'S DATE
    var day   = this.day;
    var month = this.month;
    var year  = this.year;
    if (year < 100)
        year += 1900;
    this.focusDay                           = day;
    document.calControl.month.selectedIndex = month;
    document.calControl.year.value          = year;
    displayCalendar(day, month, year);
}


function isFourDigitYear(year) {
    if (year.length != 4) {
        alert ("Sorry, the year must be four-digits in length.");
        document.calControl.year.select();
        document.calControl.year.focus();
    }
    else {
        return true;
    }
}


function selectDate() {
    var year  = document.calControl.year.value;
    if (isFourDigitYear(year)) {
        var day   = 0;
        var month = document.calControl.month.selectedIndex;
        displayCalendar(day, month, year);
    }
}


function setPreviousYear() {
    var year  = document.calControl.year.value;
    if (isFourDigitYear(year)) {
        var day   = 0;
        var month = document.calControl.month.selectedIndex;
        year--;
        document.calControl.year.value = year;
        displayCalendar(day, month, year);
    }
}


function setPreviousMonth() {
    var year  = document.calControl.year.value;
    if (isFourDigitYear(year)) {
        var day   = 0;
        var month = document.calControl.month.selectedIndex;
        if (month == 0) {
            month = 11;
            if (year > 1000) {
                year--;
                document.calControl.year.value = year;
            }
        }
        else {
            month--;
        }
        document.calControl.month.selectedIndex = month;
        displayCalendar(day, month, year);
    }
}


function setNextMonth() {
    var year  = document.calControl.year.value;
    if (isFourDigitYear(year)) {
        var day   = 0;
        var month = document.calControl.month.selectedIndex;
        if (month == 11) {
            month = 0;
            year++;
            document.calControl.year.value = year;
        }
        else {
            month++;
        }
        document.calControl.month.selectedIndex = month;
        displayCalendar(day, month, year);
    }
}


function setNextYear() {
    var year  = document.calControl.year.value;
    if (isFourDigitYear(year)) {
        var day   = 0;
        var month = document.calControl.month.selectedIndex;
        year++;
        document.calControl.year.value = year;
        displayCalendar(day, month, year);
    }
}


function displayCalendar(day, month, year) {       

    day     = parseInt(day);
    month   = parseInt(month);
    year    = parseInt(year);
    var i   = 0;
    var now = new Date();

    if (day == 0) {
        var nowDay = now.getDate();
    }
    else {
        var nowDay = day;
    }
    var days         = getDaysInMonth(month+1,year);
    var firstOfMonth = new Date (year, month, 1);
    var startingPos  = firstOfMonth.getDay();
    days += startingPos;

    // MAKE BEGINNING NON-DATE BUTTONS BLANK
    for (i = 0; i < startingPos; i++) {
        document.calButtons.elements[i].value = "  ";
        document.calButtons.elements[i].disabled = true;
			document.calButtons.elements[i].className = "submitBtn2";
    }

    // SET VALUES FOR DAYS OF THE MONTH
    for (i = startingPos; i < days; i++)  
    {
		if ((i-startingPos+1) < 10)
			document.calButtons.elements[i].value = ' '+eval(i-startingPos+1);
		else
			document.calButtons.elements[i].value = i-startingPos+1;
			
			document.calButtons.elements[i].className = "submitBtn1";

			
        document.calButtons.elements[i].onClick = "returnDate"
        document.calButtons.elements[i].disabled = false;
    }

    // MAKE REMAINING NON-DATE BUTTONS BLANK
    for (i=days; i<42; i++)  {
        document.calButtons.elements[i].value = "  ";
        document.calButtons.elements[i].disabled = true;
			document.calButtons.elements[i].className = "submitBtn2";
    }

    // GIVE FOCUS TO CORRECT DAY
    document.calButtons.elements[focusDay+startingPos-1].focus();
}


// GET NUMBER OF DAYS IN MONTH
function getDaysInMonth(month,year)  {
    var days;
    if (month==1 || month==3 || month==5 || month==7 || month==8 ||
        month==10 || month==12)  days=31;
    else if (month==4 || month==6 || month==9 || month==11) days=30;
    else if (month==2)  {
        if (isLeapYear(year)) {
            days=29;
        }
        else {
            days=28;
        }
    }
    return (days);
}


// CHECK TO SEE IF YEAR IS A LEAP YEAR
function isLeapYear (Year) {
    if (((Year % 4)==0) && ((Year % 100)!=0) || ((Year % 400)==0)) {
        return (true);
    }
    else {
        return (false);
    }
}


// SET FORM FIELD VALUE TO THE DATE SELECTED
function returnDate(inDay)
{
    var day   = parseInt(inDay);
	
	if (isNaN(day))
		return;

    var month = (document.calControl.month.selectedIndex)+1;
    var monthTxt = document.calControl.month.options[month - 1].text;
    
    var year  = document.calControl.year.value;

	//if (validDate(inDay))
	//{
	//	alert('請選擇今天及以後的日期 !');
	//	return;
	//}
		
    if ((""+month).length == 1)
    {
        month="0"+month;
    }
    if (day < 10)
    {
        day="0"+day;
    }
    if (day != null && day != 0) {
		userFormat.toLowerCase();
		if (userFormat == 'mm/dd/yyyy')
			dateField.value = month + "/" + day + "/" + year;
		else if (userFormat == 'dd/mm/yyyy')
			dateField.value = day + "/" + month + "/" + year;
		else if (userFormat == 'yyyy/dd/mm')
			dateField.value = year + "/" + day + "/" + month;
		else if (userFormat == 'yyyy/mm/dd')
			dateField.value = year + "/" + month + "/" + day;
		else if (userFormat == 'dd/mmm/yyyy')
			dateField.value = day + "/" + monthTxt.substring(0,3) + "/" + year;
		
        window.close()
    }
}

function validDate(inDay)
{
	var day = inDay;
	var month = document.calControl.month.selectedIndex;
	var year  = document.calControl.year.value;
	if (year < 100)
		year += 1900;
	
	var selDate = new Date(year, month, day)
	var now = new Date (this.year, this.month, this.day)
	if (selDate<now){
		var temp = selDate;
		return (temp >= 0);
	}
}
</SCRIPT>
</HEAD>

<BODY BGCOLOR="<?=body_color?>" onLoad="this.focus();setDate();">

<CENTER>
<FORM NAME="calControl" onSubmit="return false;">
<table border="1" align="center" cellpadding="13" cellspacing="0" bordercolor="#8E3748">
<tr>
<td>



<TABLE CELLPADDING=0 CELLSPACING=0 BORDER=0>
<TR><TD align="center" COLSPAN=7>

<SELECT STYLE="<?=top_css1?>" NAME="month" onChange='selectDate()' >
   <OPTION value="January"><?=$language["January"]?>
   <OPTION value="February"><?=$language["February"]?>
   <OPTION value="March"><?=$language["March"]?>
   <OPTION value="April"><?=$language["April"]?>
   <OPTION value="May"><?=$language["May"]?>
   <OPTION value="June"><?=$language["June"]?>
   <OPTION value="July"><?=$language["July"]?>
   <OPTION value="August"><?=$language["August"]?>
   <OPTION value="September"><?=$language["September"]?>
   <OPTION value="October"><?=$language["October"]?>
   <OPTION value="November"><?=$language["November"]?>
   <OPTION value="December"><?=$language["December"]?>
</SELECT>
<INPUT NAME="year" STYLE="<?=top_css1?>" TYPE=TEXT SIZE=5 MAXLENGTH=4 onChange="selectDate()">

</TD>
</TR>
<tr>
	<td COLSPAN=7><img src="tran.gif" width="1" height="5"></td>
</tr>
<TR>
<TD align="center" COLSPAN=7>
<INPUT TYPE=BUTTON  STYLE="<?=top_css?>" VALUE="<<"    onClick="setPreviousYear()">
<INPUT TYPE=BUTTON  STYLE="<?=top_css?>" VALUE=" <"   onClick="setPreviousMonth()">
<INPUT TYPE=BUTTON  STYLE="<?=top_css?>" VALUE="Today" id="bt_today" onClick="setToday()">
<INPUT TYPE=BUTTON  STYLE="<?=top_css?>" VALUE="> "   onClick="setNextMonth()">
<INPUT TYPE=BUTTON  STYLE="<?=top_css?>" VALUE=">>"    onClick="setNextYear()">

</TD>
</TR>
</FORM>

<FORM NAME="calButtons">

<TR HEIGHT=10><TD></TD></TR>

<TR><TD><CENTER><FONT SIZE=-1 FACE="Arial,Helv,Helvetica" color="#CCCCCC"><B><?=$language["S2"]?></B></FONT></CENTER></TD>
    <TD><CENTER><FONT SIZE=-1 FACE="Arial,Helv,Helvetica" color="#CCCCCC"><B><?=$language["M"]?></B></FONT></CENTER></TD>
    <TD><CENTER><FONT SIZE=-1 FACE="Arial,Helv,Helvetica" color="#CCCCCC"><B><?=$language["T"]?></B></FONT></CENTER></TD>
    <TD><CENTER><FONT SIZE=-1 FACE="Arial,Helv,Helvetica" color="#CCCCCC"><B><?=$language["W"]?></B></FONT></CENTER></TD>
    <TD><CENTER><FONT SIZE=-1 FACE="Arial,Helv,Helvetica" color="#CCCCCC"><B><?=$language["T2"]?></B></FONT></CENTER></TD>
    <TD><CENTER><FONT SIZE=-1 FACE="Arial,Helv,Helvetica" color="#CCCCCC"><B><?=$language["F"]?></B></FONT></CENTER></TD>
    <TD><CENTER><FONT SIZE=-1 FACE="Arial,Helv,Helvetica" color="#CCCCCC"><B><?=$language["S"]?></B></FONT></CENTER></TD></TR>


<TR><TD><INPUT TYPE="button" NAME="but0"  value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but1"  value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but2"  value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but3"  value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but4"  value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but5"  value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but6"  value="  " onClick="returnDate(this.value)"></TD></TR>

<TR><TD><INPUT TYPE="button" NAME="but7"  value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but8"  value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but9"  value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but10" value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but11" value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but12" value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but13" value="  " onClick="returnDate(this.value)"></TD></TR>

<TR><TD><INPUT TYPE="button" NAME="but14" value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but15" value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but16" value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but17" value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but18" value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but19" value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but20" value="  " onClick="returnDate(this.value)"></TD></TR>

<TR><TD><INPUT TYPE="button" NAME="but21" value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but22" value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but23" value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but24" value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but25" value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but26" value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but27" value="  " onClick="returnDate(this.value)"></TD></TR>

<TR><TD><INPUT TYPE="button" NAME="but28" value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but29" value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but30" value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but31" value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but32" value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but33" value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but34" value="  " onClick="returnDate(this.value)"></TD></TR>

<TR><TD><INPUT TYPE="button" NAME="but35" value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but36" value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but37" value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but38" value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but39" value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but40" value="  " onClick="returnDate(this.value)"></TD>
    <TD><INPUT TYPE="button" NAME="but41" value="  " onClick="returnDate(this.value)"></TD></TR>

</TABLE>

</td>
</tr>
</table>
</FORM>
</BODY>
</HTML>
