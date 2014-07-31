/*
* Tools
*/

ezAjax.tools = 
{
	/*
	 * This functions handles 2(two) checkboxes to work as radio buttons.
	 * It will assign values (0,1) to two hidden values in the container
	 * 
	 * Container: id or class of the elemene containing the two radio buttons
	 * selector: position of the checkbox clicked (values are 0 or 1)
	*/
	toggleRadioBoolean : function(container,selector)
	{
		i=0;
		$(container+" input[@type='checkbox']").each(function()
		{
			if (i != selector) 
			{
				this.checked = false;
			}
			i++;
		});
		i=0;
		$(container+" input[@type='hidden']").each(function()
		{
			if (i==selector) 
			{
				this.value =   this.value == 0 ? 1 : 0 ;
			}
			else
			{
				this.value =   0;
			}
			i++;
		});

	},
	
	toggleHiddenBoolean : function(selector)
	{
		$(selector).val( $(selector).val() == '0' ? '1' : '0' );
	},
	
	stripHTML : function(str)
	{
		var regexp = /<("[^"]*"|'[^']*'|[^'">])*>/gi;
		return str.replace(regexp,'');
	},
	
	bookmark : function bookmark(url, title)
	{
		if (window.sidebar) // firefox
		{
			window.sidebar.addPanel(title, url, "");
		}
		else if(window.opera && window.print)
		{ // opera
			var elem = document.createElement('a');
			elem.setAttribute('href',url);
			elem.setAttribute('title',title);
			elem.setAttribute('rel','sidebar');
			elem.click();
		} 
		else if(document.all)// ie
		{
			if ( typeof window.external.AddFavorite == 'unknown' )
			{
				alert("Please press CTRL+D to bookmark this page");
			}
			else
			{
				window.external.AddFavorite(location.href,document.title);
			}
		}
	},
	
	confirm : function(confMsg,url)
	{
		if (confirm(confMsg))
		{
			location.href=url;
		}	
	},
	
	checkEmail : function(email)
	{
		if ( email.length <= 3 )
		{
			return false;	
		}
		
		if ( ! email.match(/[_a-z0-9\'-]+(\.[_a-z0-9\'-]+)*@[a-z0-9-]+(\.[a-z0-9-]+)*(\.[a-z]{2,4})$/g) )
		{
			return false;
		}
		
		return true;
	},
	
	checkURL : function(isURL)
	{
		var regexp = /(http|https):\/\/(\w+:{0,1}\w*@)?(\S+)(:[0-9]+)?(\/|\/([\w#!:.?+=&%@!\-\/]))?/;
		return regexp.test(isURL);
	},
	
	/*
	* clear field when clicking in it
	*/
	
	cleared : [],
	
	clearField : function (fieldObj,newText)
	{
		if ( this.cleared[fieldObj.id] !== undefined ) 
		{
			return;
		}
		
		this.cleared[fieldObj.id] = true;
		
		fieldObj.value = newText!==undefined?newText:'';

		$(fieldObj).removeClass('fade');
		
		ezAjax.tools.setCaretTo(fieldObj, fieldObj.value.length);
		
	},
	
	insertAtCaret : function(obj, text) 
	{
		if(document.selection) { 
			obj.focus();
			var orig = obj.value.replace(/\r\n/g, "\n");
			var range = document.selection.createRange();

			if(range.parentElement() != obj) {
				return false;
			}

			range.text = text;
			
			var actual = tmp = obj.value.replace(/\r\n/g, "\n");

			for(var diff = 0; diff < orig.length; diff++) {
				if(orig.charAt(diff) != actual.charAt(diff)) break;
			}

			for(var index = 0, start = 0; 
				tmp.match(text) 
					&& (tmp = tmp.replace(text, "")) 
					&& index <= diff; 
				index = start + text.length
			) {
				start = actual.indexOf(text, index);
			}
		} else if(obj.selectionStart) {
			var start = obj.selectionStart;
			var end   = obj.selectionEnd;

			obj.value = obj.value.substr(0, start) 
				+ text 
				+ obj.value.substr(end, obj.value.length);
		}
		
		if(start != null) {
			yapp.tools.setCaretTo(obj, start + text.length);
		} else {
			obj.value += text;
		}
	},

	setCaretTo : function(obj, pos) 
	{
		if(obj.createTextRange) {
			var range = obj.createTextRange();
			range.move('character', pos);
			range.select();
		} else if(obj.selectionStart) {
			obj.focus();
			obj.setSelectionRange(pos, pos);
		}
	},
	
	/*
	* Check if enter key was hit
	*/
	
	isFormTrigger: function (e)
	{
		if ( (e.keyCode == 13 && e.shiftKey!= 1 ) || e.type == 'click' )
		{
			return true;
		}
	}

};
