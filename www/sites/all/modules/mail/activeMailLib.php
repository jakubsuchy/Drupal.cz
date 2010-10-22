<?php
// +----------------------------------------------------------------------+
// | activeMailLib 0.1                                                    |
// +----------------------------------------------------------------------+
// | Date: 17 Feb 2005                                                    |
// +----------------------------------------------------------------------+
// | License: LGPL                                                        |
// +----------------------------------------------------------------------+
// | activeMailLib is a PHP class to validate and send                    |
// | MIME E-mail messages using the PHP function mail().                  |
// +----------------------------------------------------------------------+
// | Author: Giorgos Tsiledakis <gt(at)corissia(dot)com>                  |
// +----------------------------------------------------------------------+
class activeMailLib{
//==============================================================================================
// PUBLIC (Constructor)
// param $type: sets the mail type; accepted values: plain or html; default: plain
//==============================================================================================
function activeMailLib($type="plain"){
	if ($type=="html" || $type=="plain") $this->type=$type;
}
//==============================================================================================
// PUBLIC enableAddressValidation() -> checks the validity of an email address when sending an mail
//==============================================================================================
function enableAddressValidation(){
$this->checkaddr=true;
}
//==============================================================================================
// PUBLIC enableServerValidation() -> checks the validity of an email server when sending an mail
// (not implemented for windows servers yet)
//==============================================================================================
function enableServerValidation(){
$this->checkserv=true;
}
//==============================================================================================
// PUBLIC To() -> sets the mail receiver(s)
// param $to: string or array (to send to many receivers)
//==============================================================================================
function To($to){
	if (is_array($to)){
		foreach ($to as $res){
			if ($this->mkCheck($res)) $this->mailto[] = $res;
		}
	}
	if (is_string($to)){
		trim($to);
		if ($this->mkCheck($to)) $this->mailto[]=$to;
	}
}
//==============================================================================================
// PUBLIC Cc() -> sets the Carbon Copy mail receiver
//==============================================================================================
function Cc($cc){
	if (is_string($cc)){
		trim($cc);
		if ($this->mkCheck($cc)) $this->mailcc=$cc;
	}
}
//==============================================================================================
// PUBLIC Bcc() -> sets the Blind Carbon Copy mail receiver
//==============================================================================================
function Bcc($bcc){
	if (is_string($bcc)){
		trim($bcc);
		if ($this->mkCheck($bcc)) $this->mailbcc=$bcc;
	}
}
//==============================================================================================
// PUBLIC From() -> sets the mail sender
// param $from: receiver`s mail (string)
// param $name: receiver`s name (string)
//==============================================================================================
function From($from,$name=false){
	if (is_string($from)){
		trim($from);
		if ($this->mkCheck($from)){
			$this->mailfrom=$from;
			$this->mailreply=$from;
		}
	}
	if ($name && is_string($name)){
		trim($name);
		if ($name!=" ") $this->mailfromname="\"".addslashes($name)."\"";
		else $this->mailfromname=$this->mailfrom;
	}
}
//==============================================================================================
// PUBLIC Reply() -> sets the address for a reply mail
// param $to: reply receiver`s mail (string); default: sender`s mail
//==============================================================================================
function Reply($to){
	if (is_string($to)){
		trim($to);
		if ($this->mkCheck($to)) $this->mailreply=$to;
	}
}
//==============================================================================================
// PUBLIC Subject() -> sets the mail subject
//==============================================================================================
function Subject($var=""){
	if (is_string($var)) $this->mailsubj=strtr($var,"\r\n","  "); // just one line
}
//==============================================================================================
// PUBLIC Message() -> sets the mail message body
// param $var: message as string
// param $charset: message charset
// param $encoding: message encoding
//==============================================================================================
function Message($var="",$charset="iso-8859-1",$encoding="quoted-printable"){
	if (is_string($var)) $this->mailbody=$var;
$this->charset=strtolower($charset);
$this->encoding=strtolower($encoding);
}
//==============================================================================================
// PUBLIC Attachment() -> sets a mail attachment
// param $file: the path or the url of the file to send as attachment
// param $filename: the name you want the attachment file to have
// param $maxsize: the maximal size in bytes of the attachment file
// param $disp: the disposition of the attachment file; accepted values: inline or attachment
// param $type: the type of the attachment file
//==============================================================================================
function Attachment($file,$filename=false,$maxsize=false,$disp="attachment",$type=false){
	if (!$filename) $filename=$file;
	if (is_readable($file)){ // get a local file
		if ($maxsize && $maxsize<filesize($file)){
			clearstatcache();
			return false;
		}
		else{
		$this->attfile['name'][]=$filename;
			if ($type) $this->attfile['type'][]=$type;
			else $this->attfile['type'][]="application/octet-stream";
		$this->attfile['tmp_name'][]=$file;
		$this->attfile['size'][]=filesize($file);
			if ($disp!="inline") $this->attfile['disp'][]="attachment";
			else $this->attfile['disp'][]=$disp;
		clearstatcache();
		}
	}
	else{ // get an URL (allow_url_fopen in php.ini must be true for that)
		$fp=@fopen($file,'rb');
		if ($fp){
			$this->attfile['name'][]=$filename;
				if ($type) $this->attfile['type'][]=$type;
				else $this->attfile['type'][]="application/octet-stream";
			$this->attfile['tmp_name'][]=$file;
			$this->attfile['size'][]=1024;
				if ($disp!="inline") $this->attfile['disp'][]="attachment";
				else $this->attfile['disp'][]=$disp;
			fclose($fp);
		}
		else return false;
	}
}
//==============================================================================================
// PUBLIC Receipt() -> returns a receipt when the mail is read (not suported by all mail servers)
// param $to: receipt receiver`s mail (string); default: sender`s mail
//==============================================================================================
function Receipt($to=false){
$this->mailreceipt=$this->mailfrom;
	if ($to && is_string($to)){
		trim($to);
		if ($this->mkCheck($to)) $this->mailreceipt=$to;
	}
}
//==============================================================================================
// PUBLIC Priority() -> sets the mail priority
// param $num: 1-5
//==============================================================================================
function Priority($num){
	if ($num>=1 && $num<=5) $this->mailpriority=$num;
}
//==============================================================================================
// PUBLIC Send() -> sends the mail
//==============================================================================================
function Send(){
$this->mkHeaders();
	if ($this->mailto && $this->mailfrom){
		foreach($this->mailto as $res){
		$serv=$this->mkCheckServer($res);
		$this->serv[$res]=$serv[1];
			if ($serv[0]==true) $this->mailsent[$res]=mail($res, $this->mailsubj, $this->mailbody, $this->mailheaders);
			else $this->mailsent[$res]=false;
		}
	}
}
//==============================================================================================
// PUBLIC isSent() -> checks if a mail has been sent
//==============================================================================================
function isSent($addr){
	if (@$this->mailsent[$addr]) return $this->mailsent[$addr];
	else return false;
}
//==============================================================================================
// PUBLIC checkAddress() -> checks the validity of an email address before sending an mail
//==============================================================================================
function checkAddress($addr){
$this->checkaddr=true;
$result=$this->mkCheck($addr);
$this->checkaddr=false;
return $result;
}
//==============================================================================================
// PUBLIC checkServer() -> checks the validity of an email server before sending an mail
// (not implemented for windows servers yet)
//==============================================================================================
function checkServer($addr){
$this->checkserv=true;
$result=$this->mkCheckServer($addr);
$result=$result[0];
$this->checkserv=false;
return $result;
}
//==============================================================================================
// PUBLIC getRawData() -> returns the generated headers, the mail receiver(s), subject and body
//==============================================================================================
function getRawData(){
$this->mkHeaders();
$out="";
	if ($this->mailto) {
		foreach($this->mailto as $res) $out.="TO: ".$res."<br /><br />\n";
	}
$out.="SUBJECT: ".$this->mailsubj."<br /><br />\n";
$out.="MESSAGE: ".$this->mailbody."<br /><br />\n";
$out.="HEADERS: ".htmlentities($this->mailheaders)."<br /><br />\n";
return $out;
}
//==============================================================================================
// PRIVATE VARIABLES -> do not modify them on runtime, use the finctions instesd
//==============================================================================================
var $mailvers = "activeMailLib PHP";
var $knownmail="local@localhost.com";
var $checkaddr=false;
var $checkserv=false;
var $type="plain";
var $charset="iso-8859-1";
var $encoding="quoted-printable";
var $mailto=false;
var $mailfrom=false;
var $mailfromname=false;
var $mailreply=false;
var $mailcc=false;
var $mailbcc=false;
var $mailreceipt=false;
var $mailpriority=false;
var $mailsubj="";
var $mailbody="";
var $mailheaders=false;
var $attfile=false;
var $mailsent=array();
//==============================================================================================
// PRIVATE mkCheck() -> checks the validity of a mail address
//==============================================================================================
function mkCheck($var){
$temp="^[_a-zA-Z0-9-](\.{0,1}[_a-zA-Z0-9-])*@([a-zA-Z0-9-]{2,}\.){0,}[a-zA-Z0-9-]{3,}(\.[a-zA-Z]{2,4}){1,2}$";
	if ($this->checkaddr){
		if (ereg($temp,$var)) return true;
		else{
			$this->mailsent[$var]=false;
			return false;
			}
	 }
	 else return true;
}
//==============================================================================================
// PRIVATE mkCheckServer() -> checks the validity of a mail server (NOT ON WINDOWS)
//==============================================================================================
function mkCheckServer($email){
	// no validation if OS is Windows, return true
	if(defined("PHP_OS")  && !strcmp(substr(PHP_OS,0,3),"WIN")) return array(true, "This method is not supported on Windows systems");
list($user,$domain) = split("@",$email,2);
$mxlist=false;
	if ($this->checkserv){
		@getmxrr($domain,$mxlist);
		$result=array(false,$domain." Mailserver not found");
		if ($mxlist){
			foreach($mxlist as $mx){
				$fp=fsockopen($mx,25,$errno,$errstr,20);
					if (!$fp) continue;
				socket_set_blocking($fp, false);
				$s=0;
				$c=0;
				$out="";
				do{
				$out=fgets($fp,2500);
					if (ereg("^220",$out)){
						$s=0;
						$out="";
						$c++;
					}
					else{
						if (($c>0 && $out=="")) break;
						else $s++;
						if ($s>9999) break;
					}
				} while ($out=="");
				socket_set_blocking($fp, true);
				fputs($fp,"HELO ActiveMail\n");
				$out=fgets($fp,3000);
				fputs($fp,"MAIL FROM: $this->knownmail\n");
				$out=fgets($fp,3000);
				fputs($fp,"RCPT TO: $email\n");
				$out=fgets($fp,3000);
					if (ereg("^250",$out)) $result=array(true,$out);
					else $result=array(false,$out);
				fputs($fp,"quit\n");
				fclose($fp);
				if ($result[0]) break;
			}
		}
		else $result=array(false,$email." Function getmxrr() not supported or Mailserver not found");
	}
	else $result=array(true,$email." Mailserver not checked");
return $result;
}
//==============================================================================================
// PRIVATE mkHeaders() -> generates the mail headers
//==============================================================================================
function mkHeaders(){
$this->headers["Mime-Version"]="1.0";
$this->headers["Content-Type"]="text/$this->type; charset=\"$this->charset\"";
$this->headers["Content-Transfer-Encoding"]="$this->encoding";
	if ($this->mailfrom){
		if ($this->mailfromname) $this->headers["From"] = $this->mailfromname." <".$this->mailfrom.">";
		else $this->headers["From"] = $this->mailfrom;
	}
	if ($this->mailcc) $this->headers["Cc"] = $this->mailcc;
	if ($this->mailbcc) $this->headers["Bcc"] = $this->mailbcc;
	if ($this->mailvers) $this->headers["X-Mailer"] = $this->mailvers;
	if ($this->mailreply) $this->headers["Reply-To"]=$this->mailreply;
	if ($this->mailreceipt){
		$this->headers["Disposition-Notification-To"]=$this->mailreceipt;
		$this->headers["X-Confirm-Reading-To"]=$this->mailreceipt;
	}
	if ($this->mailpriority){
		switch($this->mailpriority){
		case 1:
		$this->headers["Priority"]="High";
		$this->headers["X-Priority"]="1";
		$this->headers["X-MSMail-Priority"]="Highest";
		break;
		case 2:
		$this->headers["Priority"]="urgent";
		$this->headers["X-Priority"]="2";
		$this->headers["X-MSMail-Priority"]="High";
		break;
		case 3:
		$this->headers["Priority"]="normal";
		$this->headers["X-Priority"]="3";
		$this->headers["X-MSMail-Priority"]="Normal";
		break;
		case 4:
		$this->headers["Priority"]="non-urgent";
		$this->headers["X-Priority"]="4";
		$this->headers["X-MSMail-Priority"]="Low";
		break;
		case 5:
		$this->headers["Priority"]="non-urgent";
		$this->headers["X-Priority"]="5";
		$this->headers["X-MSMail-Priority"]="Lowest";
		break;
		}
	}
$this->mkAttachment();
	reset($this->headers);
	$this->mailheaders="";
		while(list($headername,$value) = each($this->headers)) {
			if ($value) $this->mailheaders.="$headername: $value\n";
		}
}
//==============================================================================================
// PRIVATE mkAttachment() -> generates the mail body in case of an attachment
//==============================================================================================
function mkAttachment(){
$boundary="--".md5(uniqid(time()));
	if($this->attfile){
	$this->headers["Content-Type"]="multipart/mixed;\n boundary=\"".$boundary."\"; type=\"multipart/mixed\"";
	$mailbody="This is a multi-part message in MIME format.\n";
	$mailbody.="--".$boundary;
	$mailbody.="\nContent-Type: text/$this->type; charset=\"$this->charset\"\nContent-Transfer-Encoding: $this->encoding\n\n".$this->mailbody."\n";
	$i=0;
	$sep= chr(13) . chr(10);
		for ($x=0;$x<count($this->attfile['name']);$x++){
			$base=$this->attfile['name'][$x];
			$type=$this->attfile['type'][$x];
			$disp=$this->attfile['disp'][$x];
			$subhdr="--".$boundary."\nContent-type: $type;\n name=\"$base\"\nContent-Transfer-Encoding: base64\nContent-Disposition: $disp;\n  filename=\"$base\"\n";
			$subheader[$i++]=$subhdr;
			$fsize=$this->attfile['size'][$x]+1;
			$fp=fopen($this->attfile['tmp_name'][$x],'rb');
			$filecont="";
			while (!feof($fp)) $filecont.= fread($fp, $fsize);
			$subheader[$i++]=chunk_split(base64_encode($filecont));
			fclose($fp);
		}
	$mailbody.= implode($sep, $subheader);
	$mailbody.="\n--".$boundary."--";
	$mailbody.="\n-- End --";
	$this->mailbody=$mailbody;
	}
}
}
?>
