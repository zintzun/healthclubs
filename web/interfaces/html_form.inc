<?php

	/***************************************************
	* This function combines yaml SPYC parser with below
	* form class
	*/

	function display_form($form_action,$form_id,$form_title,$dat_file)
	{
		// Load the YAML dat file
		$FORM_VALS = Spyc::YAMLLoad($dat_file);

		// Initiate the form		
		$FORM = new form($form_action,$form_id,$form_title);

		// Loop through data structure and create the form HTML
		foreach ( $FORM_VALS as $R )
		{
			if ( $R['type'] == 'form_separator' )
			{
				$FORM->add('form_separator');
			}
			else if ( $R['type'] == 'form_description' )
			{
				$FORM->add($R['type'],$R['desc']);
			}
			else if ($R['type'] == 'back_next_buttons')
			{
				$FORM->add_group('back-next', 'button','','&laquo; Back',array('onClick'=>'history.go(-1)'));
				$FORM->add_group('back-next', 'submit','do_next','Next &raquo;');
			}
			else
			{
				$FORM->add
				(
					$R['type'],
					isset($R['name'])?$R['name']:'',
					isset($R['value'])?$R['value']:'',
					isset($R['attributes'])?$R['attributes']:'',
					isset($R['inner_label'])?$R['inner_label']:'',
					isset($R['label'])?tip($R['tip']).$R['label']:''
				);

				if ( isset($R['label']) )
				{
					$label = substr($R['label'],0,-1);
				}
				else if ( isset($R['inner_label']) )
				{
					$label = substr($R['inner_label'],0,-1);
				}
				else
				{
					$label = ucwords($R['name']);
				}

				$FORM->add_rule('require',$R['name'],"Please enter ".$label,0,-1);
			}
		}

		// Print the form to screen	
		print $FORM->render();

	}

	/***************************************************
	* Abstracted form class
	*/

	class form
	{

		var $data = array();
		var $groups = array();
		var $action = '/';
		var $title  = '';
		var $rules = array();
		var $form_id = '';
		var $method  = 'POST';
		var $rte_width  = 600;
		var $rte_height = 400;

		/*******************************************
		* Constructor
		*/

		function form($action,$form_id,$title='',$method='post')
		{
			$this->action = $action;
			$this->form_id  = $form_id;
			$this->title  = $title;
			$this->method  = $method;
		}

		/*******************************************
		* get final HTML
		*/

		function render()
		{
			
			$html  = $this->open();
			
			foreach ($this->data as $group_id => $item)
			{
				if ( ! is_array($item) )
				{
					$html .= $item;
				}
				else
				{
					$group_html = '';
					
					foreach ($item as $group_item)
					{
						$group_html .= $group_item['input'];
					}
					
					 $html .= $this->wrapper("$group_id",$group_html);

				}
			}
			
			$html .= $this->close() . $this->render_rules();
			
			return $html;
		}

		/*******************************************
		* Open form
		*/

		function open()
		{
			$html = "<div class=\"std-form\">\n";
			$html .= "<form method=\"{$this->method}\" action=\"$this->action\" id=\"{$this->form_id}\" onSubmit=\"return validate_form();\">\n";
			
			if ( $this->title )
			{
				$html .= "<fieldset>\n<legend>$this->title</legend><div class=\"fieldset-inner\">\n";
			}

			$html .= '<table width="100%">';

			return $html;
		}

		/*******************************************
		* Close form
		*/

		function close()
		{
			$html = '';
			
			if ( $this->title )
			{
				$html .= "</table></div></fieldset>";
			}
			
			return $html."</form>\n</div>\n<!-- close form -->\n";	
		}

		/*******************************************
		* Add form element to stack
		*/

		function add($type,$name='',$value='',$extras=false,$inner_label='',$outer_label='')
		{
			$this->data[] = $this->wrapper("$type-tr",$this->get($type,$name,$value,$extras,$inner_label),$outer_label);
		}

		/*******************************************
		* Create grouping of componenets on a form
		*/

		function add_group($group_id,$type,$name='',$value='',$extras=false,$text='')
		{
			$this->data[$group_id][] = array
			(
				'input' => $this->get($type,$name,$value,$extras,$text),
				'type'  => $type,
				'name'  => $name,
			);
		}

		/*******************************************
		* Render single element of form
		*/

		function get($type,$name='',$value='',$extras=false,$text_or_array='')
		{

			static $id_count = 0;

			// Id this val is availbel via post then get it from there
			// Otherwise use argument as default value
			$value = get_var($name,$value);	

			$id_count++;

			$attribs = array
			(
				'type'  => $type,
				'name'  => $name,
				'id'    => "id-{$name}",
				'class' => "input-$type id-{$name}"
			);

			if ( $extras )
			{
				$attribs = array_merge($attribs,$extras);
			}

			switch($type)
			{

				case 'form_separator':

					return  '<div class="form-seperator"><div>';

				case 'rich_text':

					return "<div class=\"tag {$name}-tag\">".$text_or_array.'</div>'.$this->get_rich_text_editor($name,$value);

				case 'form_description':

					return  '<div class="form-description">'.$name.'</div>';

				case 'form_body':

					return  '<div class="form-body">'.$name.'</div>';

				case 'textarea':
				
					return "<div class=\"tag {$name}-tag\">".$text_or_array.'</div><textarea '.$this->attribs($attribs).'>'.$value.'</textarea>';	
				
				break;	

				case 'radio':				
				case 'checkbox':

					$attribs['id'] = "id-{$name}-$id_count";

					$attribs['value'] = $value;
					$attribs['type']  = $type;

					$selected = '';
					if ( $value && $type == 'checkbox' )
					{
						$selected = 'checked';						
					}
					
					if ( $value && $type == 'radio' )
					{
						$selected = 'radio';
					}

					return '<input '.$this->attribs($attribs)." $selected/><label for=\"id-{$name}-$id_count\" class=\"input-$type-label id-{$name}-$id_count-label\">$text_or_array</label>";	
				
				break;	

				case 'select':

					$html = '<select '.$this->attribs($attribs).">";

					if ( isset($text_or_array[$value]) )
					{
						$html .= "<option value=\"$value\" class=\"selected-option\">{$text_or_array[$value]}</option>";
					}

					foreach ( $text_or_array as $option_value => $option_text )
					{					
						$html .= "<option value=\"$option_value\">$option_text</option>";
					}

					$html .= '</select>';

					return $html;

				case 'password':
				case 'text':

					$attribs['value'] = $value;
					$attribs['type']  = $type;

					return "<div class=\"tag {$name}-tag\">".$text_or_array.'</div><input '.$this->attribs($attribs).'/>';	

				default:
	
					$attribs['value'] = $value;
					$attribs['type']  = $type;

					return '<input '.$this->attribs($attribs).'/>';	

				break;

				
			}
			
		}

		/*******************************************
		* Render element attributes
		*/

		function attribs($attribs)
		{
			$html = '';
			
			foreach ( $attribs as $attrib => $val )
			{
				$html .= "$attrib=\"$val\" ";
			}
			
			return $html;
		}

		/*******************************************
		* Render element styling wrapper
		*/

		function wrapper($class,$content,$label='')
		{
			//if ( preg_match("/(form_description|form_separator)/i",$class) )
			//{
				//return "<tr class=\"std-form-item $class\"><td colspan=\"2\">$content</td></tr>\n";
			//}
			//else
			//{
				return "<tr class=\"std-form-item $class\"><td nowrap valign=\"top\" style=\"padding-top: 4px;\" class=\"label\">$label</td><td>$content</td></tr>\n";	
			//}
		}

		/*******************************************
		* Add input rule
		*/

		function add_rule($rule,$target_var,$display,$options='')
		{
			$this->rules[] = array
			(
				'rule'    => $rule,
				'target'  => $target_var,
				'options' => $options,
				'display' => $display,
			);
		}

		function render_rules()
		{

$html = <<<END
<script type="text/javascript">
function validate_form()
{\n
END;
			
			foreach ($this->rules as $rule)
			{
				switch($rule['rule'])
				{
	
					case 'require':
						
$html .= <<<END
if ( document.getElementById('id-{$rule['target']}').value == '' )
{
	alert("{$rule['display']}");
	return false;
}\n
END;
						
						break;



				}
			}

$html .= <<<END
return true;
}
//setTimeout("window.scrollTo(0,0)",100);
</script>
END;

			return $html;
			
		}

		function set_rte_size($width,$height)
		{
			$this->rte_width  = $width;
			$this->rte_height = $height;
		}

		function get_rich_text_editor($var_name, $value = '')
		{

			static $first_rte;

			if ( ! isset($first_rte) )
			{
				$first_rte = $var_name;
			}

			$value = htmlspecialchars(preg_replace("/[\r\n]/",'',$value));

return <<<END
<div class="rte $var_name-rte">
<script language="JavaScript" type="text/javascript">
<!--

initRTE("/third_party/rte/images/", "/third_party/rte/", "/third_party/rte/rte.css", false);

var $var_name = new richTextEditor('$var_name');

$var_name.html = '$value';
$var_name.width     = $this->rte_width;
$var_name.height    = $this->rte_height;

$var_name.build();

//-->
</script>
</div>
END;

		}

	}

?>