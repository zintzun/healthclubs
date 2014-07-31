<?php

	function get_range_sql($range)
	{
		
		// If current month
		if ( $range == 0 )
		{
			$sql = "bl.creation_date >= DATE_ADD(LAST_DAY(DATE_SUB(LAST_DAY(CURDATE()),INTERVAL 1 MONTH)),INTERVAL 1 DAY)";
		}
		else
		{
			$range = $range*-1;
			
			// If range over 20 years ago!
			if ( $range >=  240 )
			{
				$sql = "bl.creation_date <= CURDATE()";
			}
			// Go back to the month specified
			else
			{
				$sql = "bl.creation_date >= DATE_ADD(LAST_DAY(DATE_SUB(LAST_DAY(DATE_SUB(CURDATE(),INTERVAL $range MONTH)),INTERVAL 1 MONTH)),INTERVAL 1 DAY) AND bl.creation_date <= LAST_DAY(DATE_SUB(LAST_DAY(CURDATE()),INTERVAL $range MONTH))";
			}
		}

		return $sql;
	}
	
	function billing_date_select()
	{
		global $TARGET;

		$s = isset($_REQUEST['range']) ? $_REQUEST['range'] : 0;

		if ( $s==0 ) { $selected_option = date('F Y')." (Current Month)"; }
		if ( $s==-10000 ) { $selected_option = "All History"; }

		$begin_at = strtotime(BILLING_HISTORY_START);

		?><select name="range" id="date-selector" onChange="change_date()"><?
		?><option value="0" <?= $s==0?'selected':'' ?>>Current Month</option><?
	
		for ( $i=1; true; $i++ )
		{
			$ts = strtotime("-{$i} month");
			
			if ( $ts < $begin_at ) break;
			
			if ( $s=="-$i" )
			{
				$selected_option = date('F Y',$ts);
			}
			
			?><option value="-<?=$i?>" <?= $s=="-$i"?'selected':'' ?>><?= date('F Y',$ts) ?></option><?
	
		}
		?><option value="-10000" <?= $s=="-10000"?'selected':'' ?>>show all history</option><?
		?></select>
		<script>
			function change_date()
			{
				location.href = '/<?= $TARGET->action_orig ?>?range='+$('#date-selector').val();	
			}
		</script>
		
		<?

		return $selected_option;

	}

	/***************************************************************
	* Constructs biz link. Parameter is a businness instance(biz_details)
	*/

	function get_biz_link($biz)
	{
    if ($biz)
    {
			include "lib/state_utils.inc";

			return 
				 '/'.$biz->type
				.'/'.get_long_state($biz->state)
				.'/'.ucwords(str_replace(' ','-',$biz->city))
				.'/'.$biz->biz_id
				.'/'.ucwords(str_replace(' ','-',$biz->name));
		}
	}

	/***************************************************************
	* Get first match of regular expression from string
	*/

	function get_regex($regex,$str)
	{
		if (preg_match($regex, $str, $matches))
		{
			return $matches[1];
		}
		else
		{
			return false;
		}
	}

	/***************************************************************
	* Tools to extract text from markup
	*/

	function get_attribute($tag, $attrib, $str)
	{
		$regex = "/\<$tag.*$attrib\=(?:\"|')(.*)(?:\"|')[\s\>]/Us";
		return get_regex($regex, $str);
	}

	/***************************************************************
	* Will get the contents of a tag from a string of markup
	* Example:  $test = get_tag('heading', $file_contents, 'id="1"');
	* Will get the contents of the heading tag whick contains id="1"
	*/
	function get_tag($tag, $str, $tag_contains = '')
	{
		if ( isset($tag_contains) )
		{
			$regex = "/\<$tag.*".preg_quote($tag_contains).".*\>(.*)\<\/$tag\>/Uis";
		}
		else
		{
			$regex = "/\<$tag.*\>(.*)\<\/$tag\>/Uis";
		}
		return get_regex($regex, $str);
	}

	/***************************************************************
	* Really really safe encoding that can be passed around in URL's
	* DB's etc
	*/

	function very_safe_encode($nvp_str)
	{
		return str_replace('%','_PCT_',rawurlencode($nvp_str));
	}

	function very_safe_decode($nvp_str)
	{
		return rawurldecode(str_replace('_PCT_','%',$nvp_str));
	}

	/****************************************************************
	* Create a random string)
	*/

	function generate_secret($type='mixed',$length=10)
	{
		$ss = '';

		switch ( $type )
		{

			case 'lower':
				$secret_chars = "qwertyuopasdfghjkzxcvbnm";
				break;

			case 'numeric':
				$secret_chars = "0123456789";
				break;

			default:
				$secret_chars = "QWERTYUPASDFGHJKXCVBNMqwertyuopasdfghjkzxcvbnm23456789";
				break;
		}

		for ( $i = 0; $i < $length; $i ++ ) $ss .= $secret_chars[rand(0,strlen($secret_chars)-1)];
		return $ss;
	}

   /**
     * Format a number of bytes into a human readable format.
     * Optionally choose the output format and/or force a particular unit
     *
     * @param   int     $bytes      The number of bytes to format. Must be positive
     * @param   string  $format     Optional. The output format for the string
     * @param   string  $force      Optional. Force a certain unit. B|KB|MB|GB|TB
     * @return  string              The formatted file size
     */
    function filesize_format($bytes, $format = '', $force = '')
    {
        $force = strtoupper($force);
        $defaultFormat = '%01d %s';
        if (strlen($format) == 0)
            $format = $defaultFormat;
 
        $bytes = max(0, (int) $bytes);
 
        $units = array('B', 'KB', 'MB', 'GB', 'TB', 'PB');
 
        $power = array_search($force, $units);
 
        if ($power === false)
            $power = $bytes > 0 ? floor(log($bytes, 1024)) : 0;
 
        return sprintf($format, $bytes / pow(1024, $power), $units[$power]);
    }

	function format_link_subject($subject)
	{
		return sanitize_text($subject,'-');
	}

	function sanitize_text($subject,$spaces_to='-',$allowed='\-\_\.')
	{
		return preg_replace('/\s+/',$spaces_to,preg_replace("/[^a-zA-Z0-9\ {$allowed}]/",'',trim($subject)));
	}

	function highlight_content($content,$searchterm,$highlight_tag="u")
	{
		if ( ! $searchterm = trim($searchterm) ) return $content;

		// collect all tags out of content into an array
		preg_match_all("/\<.*\>/Usi",$content,$tags);

		if ( isset($tags[0]) )
		{
			$i = 0;
			foreach ($tags[0] as $tag)
			{
				$content = str_replace($tag, "T!~!GX{$i}X", $content);
				$i++;
			}
		}

		// this adds the highlight
		$content = preg_replace('/('.trim(sanitize_text($searchterm,'|'),"-_.").')/i',"<$highlight_tag>\\1</$highlight_tag>",$content);

		// (only if there are tags!)
		if ( isset($tags[0]) )
		{
			// replaces the array key placeholders with their repective html tags
			$i = 0;
			foreach ($tags[0] as $tag)
			{
				$content = str_replace("T!~!GX{$i}X",$tag,$content);
				$i++;
			}
		}
		return $content;
	}

	function format_date_ts($timestamp,$type='long')
	{
		$fmt_type = array
		(
			'long' => 'g:ia T - D jS M, Y',
			'post' => 'g:ia T',
		);
		
		return date($fmt_type[$type],$timestamp);
	}
	
	function format_date($date,$type='long')
	{
		return format_date_ts(strtotime($date),$type);
	}

	function makeClickableLinks($text) 
	{
	
	  $text = eregi_replace('(((f|ht){1}tp://)[-a-zA-Z0-9@:%_\+.~#?&//=]+)',
	    '<a href="\\1">\\1</a>', $text);
	  $text = eregi_replace('([[:space:]()[{}])(www.[-a-zA-Z0-9@:%_\+.~#?&//=]+)',
	    '\\1<a href="http://\\2">\\2</a>', $text);
	  $text = eregi_replace('([_\.0-9a-z-]+@([0-9a-z][0-9a-z-]+\.)+[a-z]{2,3})',
	    '<a href="mailto:\\1">\\1</a>', $text);
	  
	  return $text;
	
	}

	function replace_links($src,$new_link='',$new_text='') 
	{
		$src = preg_replace("/(((f|ht){1}tp:\/\/)[-a-zA-Z0-9@:%_\+.~#?&\/\/=]+)/",($new_link?'<a href="'.$new_link.'">':'').$new_text.($new_link?'</a>':''),$src);
		$src = preg_replace("/(www.[-a-zA-Z0-9@:%_\+.~#?&\/\/=]+)/",($new_link?'<a href="'.$new_link.'">':'').$new_text.($new_link?'</a>':''),$src);	  
	  	return $src;
	}

	function format_url_name($name,$url='')
	{
		if ( $url )
		{
			return "<a href=\"$url\">$name</a>";
		}
		else
		{
			return $name;
		}
	}

	function base36($value)
	{ 
		$baseChars = array('0', '1', '2', '3', '4', '5',  '6', '7', '8', '9', 'a', 'b',  'c', 'd', 'e', 'f', 'g', 'h',  'i', 'j', 'k', 'l', 'm', 'n',  'o', 'p', 'q', 'r', 's', 't',  'u', 'v', 'w', 'x', 'y', 'z'  ); 
		
		$remainder = 0; 
		$newval = ""; while ( $value > 0 ) 
		{ 
			$remainder = $value % 36; 
			$value = ( ($value - $remainder) / 36 ); 
			$newval .= $baseChars[$remainder]; 
		} 
		
		return strrev($newval); 
	}

	function ordinal_suffix($n)
	{
		if(10 < ($n%100) && ($n%100) < 14) { $s="th"; }
		else
		{
			switch($n % 10) 
			{
				case 1:  $s="st"; break;
				case 2:  $s="nd"; break;
				case 3:  $s="rd"; break;
				default: $s="th"; break;
			}
		}
		return number_format($n) . $s;
	}

	/**
	 * Take a submit time and format for web output
	 */

	function format_submit_time($submit_time)
	{
		$num_seconds = time() - strtotime($submit_time);
		
		$num_mins = floor($num_seconds/60);
		
		$date = format_s_ago($num_mins ,'minute');

		if ( $num_mins == 0 )
		{
			$date = format_s_ago($num_seconds ,'second');
		}

		if ( $num_mins > 60 )
		{
			$date = format_s_ago(round($num_mins/60),'hour');
		}
		
		if ( $num_mins > 1440 )
		{
			$date = format_s_ago(round($num_mins/(60*24)),'day');
		}
		
		if ( $num_mins > 43200 )
		{
			$date = format_s_ago(round($num_mins/(60*24*30)),'month');
		}
		
		if ( $num_mins > 518400 )
		{
			$date = format_s_ago(round($num_mins/(60*24*30*365)),'year');
		}
		
		return $date;
	}

	function format_s($num,$unit)
	{
		return number_format($num). " $unit" . ($num == 1 ? '':'s');
	}

	function format_s_ago($num,$unit)
	{
		return format_s($num,$unit).' ago';
	}

	function clean_up_str($str)
	{
		$str = str_replace("’","'",$str);
		$str = str_replace("“",'"',$str);
		$str = str_replace("”",'"',$str);
		//$str = str_replace("?s","'s",$str);
		$str = preg_replace('/\?(\w)/',"'\\1",$str);
		return trim($str);
	}

	function strip_bad_chars($str)
	{
		return preg_replace("/[^a-zA-Z0-9\~\!\@\#\$\%\^\&\*\(\)\_\+\=\-\{\}\[\]\|\"\'\:\;\?\/\>\.\<\,\`\ \r\n\/]/",'',$str);	
	}
	
	function alternate($alt2,$alt1='',$scope='global')
	{
		static $alts;
		
		if ( isset($alts[$scope]) )
		{
			unset($alts[$scope]);
			return $alt2;
		}
		else
		{
			$alts[$scope] = true;
			return $alt1;
		}
	}

	/**
	 * abstractify some text
	 * @param string $string - the string to cut short
	 * @param string $num_words - the number of WORDS to cut off at
	 * @return string - the abstracted string
	 */
	function abstractify($string,$num_words=30,$lowercase=true)
	{
		$string = strip_tags(preg_replace("/[\r\n\s]+/",' ',$string));

		if ( $string && $words = split(' ',$string) )
		{
			$abstract = '';
			for ($i=0; $i < $num_words; $i++ )
			{
				if ( ! isset($words[$i]) ) 
				{
					break;
				}
				
				$abstract .= "$words[$i] ";
			}

			if ( $lowercase )
			{
				return ucfirst(strtolower(trim($abstract)));
			}
			else
			{
				return trim($abstract);
			}
		}
	}

	function merge_snippet($merge_vals,$snippet)
	{
		foreach ( $merge_vals as $merge_name => $merge_value )
		{
			$snippet = str_replace('{'.$merge_name.'}',$merge_value,$snippet);
		}
		return $snippet;
	}
	
	function padd_keywords($str)
	{
		if ( $words = preg_split('/[^a-zA-Z0-9]/',$str) )
		{
			$padded = array();
			foreach ( $words as $word )
			{
				$word = strtolower($word);
				if ( ! is_stopword($word) )
				{
					$padded[] = $word.(strlen($word) == 3?'x':'');
				}
			}
		}

		if ( $padded )
		{
			return implode(' ',$padded);
		}
		
	}

	function is_stopword($str)
	{
		if ( strlen($str) <= 2 )
		{
			return true;	
		}
		
		switch($str)
		{
			case 'all':
			case 'and':
			case 'are':
			case 'but':
			case 'etc':
			case 'few':
			case 'for':
			case 'get':
			case 'had':
			case 'has':
			case 'her':
			case 'him':
			case 'his':
			case 'how':
			case 'inc':
			case 'its':
			case 'ltd':
			case 'may':
			case 'nor':
			case 'not':
			case 'now':
			case 'off':
			case 'one':
			case 'our':
			case 'out':
			case 'own':
			case 'per':
			case 'put':
			case 'see':
			case 'she':
			case 'six':
			case 'ten':
			case 'the':
			case 'too':
			case 'top':
			case 'two':
			case 'via':
			case 'was':
			case 'who':
			case 'why':
			case 'yet':
			case 'you':
			case 'with':
			case 'they':
			case 'this':
			case 'from':
			case 'online':
			case 'new':
			case 'real':
			case 'best':
			case 'county':
			case '2008':
			case '2009':
			case '2010':
			case 'there':
			case 'which':
			case 'any':
			case 'very':
			case 'page':
			case 'all':
			case 'also':
			case 'am':
			case 'an':
			case 'and':
			case 'any':
			case 'are':
			case 'as':
			case 'ask':
			case 'at':
			case 'be':
			case 'but':
			case 'by':
			case 'can':
			case 'cannot':
			case 'cant':
			case 'co':
			case 'did':
			case 'do':
			case 'does':
			case 'done':
			case 'eg':
			case 'et':
			case 'etc':
			case 'ex':
			case 'for':
			case 'from':
			case 'get':
			case 'gets':
			case 'go':
			case 'goes':
			case 'got':
			case 'had':
			case 'has':
			case 'have':
			case 'he':
			case 'her':
			case 'hi':
			case 'him':
			case 'his':
			case 'i\'d':
			case 'i\'ll':
			case 'i\'m':
			case 'i\'ve':
			case 'ie':
			case 'if':
			case 'in':
			case 'inc':
			case 'into':
			case 'is':
			case 'isn\'t':
			case 'it':
			case 'it\'d':
			case 'it\'ll':
			case 'it\'s':
			case 'its':
			case 'itself':
			case 'let':
			case 'let\'s':
			case 'may':
			case 'me':
			case 'my':
			case 'nd':
			case 'new':
			case 'no':
			case 'non':
			case 'nor':
			case 'not':
			case 'of':
			case 'off':
			case 'oh':
			case 'ok':
			case 'old':
			case 'on':
			case 'or':
			case 'our':
			case 'out':
			case 'own':
			case 'per':
			case 'que':
			case 'qv':
			case 'rd':
			case 're':
			case 'say':
			case 'see':
			case 'she':
			case 'so':
			case 'sub':
			case 'such':
			case 'sup':
			case 'sure':
			case 't\'s':
			case 'th':
			case 'that':
			case 'that\'s':
			case 'thats':
			case 'the':
			case 'their':
			case 'theirs':
			case 'them':
			case 'themselves':
			case 'then':
			case 'thence':
			case 'there':
			case 'there\'s':
			case 'thereafter':
			case 'thereby':
			case 'therefore':
			case 'therein':
			case 'theres':
			case 'thereupon':
			case 'these':
			case 'they':
			case 'they\'d':
			case 'they\'ll':
			case 'they\'re':
			case 'they\'ve':
			case 'think':
			case 'this':
			case 'those':
			case 'thus':
			case 'to':
			case 'too':
			case 'un':
			case 'unto':
			case 'up':
			case 'upon':
			case 'us':
			case 'use':
			case 'uses':
			case 'via':
			case 'viz':
			case 'vs':
			case 'want':
			case 'wants':
			case 'was':
			case 'way':
			case 'we':
			case 'we\'d':
			case 'we\'ll':
			case 'we\'re':
			case 'we\'ve':
			case 'were':
			case 'what':
			case 'whatever':
			case 'when':
			case 'whence':
			case 'whenever':
			case 'where':
			case 'whereafter':
			case 'whereas':
			case 'whereby':
			case 'wherein':
			case 'whereupon':
			case 'wherever':
			case 'whether':
			case 'which':
			case 'while':
			case 'whither':
			case 'who':
			case 'whoever':
			case 'whole':
			case 'whom':
			case 'whose':
			case 'why':
			case 'will':
			case 'wish':
			case 'with':
			case 'within':
			case 'without':
			case 'wonder':
			case 'would':
			case 'would':
			case 'yet':
			case 'help':
			case 'need':
			case 'your':
			return true;	
		}
	}

	function get_set_sql($parms)
	{
		global $db;
		
		$sql = '';
		foreach ( $parms as $field => $val )
		{
			if ( $val == 'NOW()' )
			{
				$sql .= "$field = ".$db->escape($val).", ";
			}
			else
			{
				$sql .= "$field = '".$db->escape($val)."', ";
			}
		}
		
		return substr($sql,0,-2);
	}
	
	/**
	 * Function to create tag cloud HTML
	 *
	 * Expecting: 
	 * tags
	 * (
	 * // tag => count
	 *    'hello' => 23,
	 *    'world' => 2,
	 * )
	 *
	 * Note link path looks something liek this...
	 * (%s is where the tag gets put in the second argument)
	 *
	 * "go/to/%s/page
	 *
	 * @return html
	 */

	function create_tag_cloud_html($tags, $link_path)
	{
global $TARGET;
		// Default font sizes
		$min_font_size = 12;
		$max_font_size = 30;
		
		$minimum_count = min(array_values($tags));
		$maximum_count = max(array_values($tags))+5;
		$spread = $maximum_count - $minimum_count;
		
		if($spread == 0) 
		{
		    $spread = 1;
		}
		
		foreach ( $tags as $key => $val )
		{
			if ( rand(0,1000) > 500 )
			{
				$tags[$key] += $spread/2;
			}
		}
				
		$cloud_html = '';
		$cloud_tags = array(); // create an array to hold tag code
		foreach ($tags as $tag => $count) 
		{
			
			$tool_tip = str_replace(array('<NUMBER>','<CITY>','<LISTING_TYPE>'),array(round($count),$tag,ucwords(str_replace('-',' ',$TARGET->arg1))),TAG_CLOUD_DESCRIPTOR);
			
			$size = $min_font_size + ($count - $minimum_count) 
				* ($max_font_size - $min_font_size) / $spread;
			$cloud_tags[] = '<a style="font-size: '. floor($size) . 'px' 
				. '" class="tag_cloud" href="' . ucfirst(sprintf($link_path,format_link_subject($tag))).'" title="' . $tool_tip  . '">' 
				. htmlspecialchars(stripslashes($tag)) . '</a>';
		}
		$cloud_html = join(" ", $cloud_tags) . " ";

		return $cloud_html;	
	}
