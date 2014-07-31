var ezAjax = 
{

	isIE : navigator.appName == 'Microsoft Internet Explorer',
	isWebKit : navigator.userAgent.indexOf('WebKit') > -1,
	isOpera : navigator.userAgent.indexOf('Opera') > -1,
	siteWidth : 1007,
	
	/*
	* Unused for the moment
	*/
	
	init : function()
	{
	},
	
	/*
	* Abstracted ajax function
	*/

	ajax : function ( ajaxFunc, sendVars, callbackFunc , statusMsg, hideLoader )
	{
		if ( hideLoader === undefined )
		{
			ezAjax.showLoading(statusMsg);
		}

		$.post
		(
			// Ajax script to call
			"/ajax/"+ajaxFunc,
			
			// Variables to send to the server
			sendVars,

			// Response from the server
			function(response)
			{

				if ( hideLoader === undefined )
				{
					ezAjax.hideLoading();
				}

				if ( typeof callbackFunc == 'function' )
				{
					callbackFunc(response);
				}
				else if ( callbackFunc !== undefined && callbackFunc !== false )
				{
					eval('ezAjax.'+callbackFunc)(response);
				}
			}, 
			"json"
		);
	},

	/*
	* Center any element on the page
	*/

	centerIt : function(itsId)
	{

		// Hmm, some wierdness going on with opera here
		var height = ezAjax.isOpera ? 600 : $(window).height();

		$(itsId).css
		({
			top: ((height/2)-($(itsId).height()/2)-(height*0.1))+$(document).scrollTop(),
			left: (ezAjax.siteWidth/2)-($(itsId).width()/2)
		});
	},

	/*
	* Show ajax loading and block UI
	*/

	showLoading : function(str)
	{
		if ( str === undefined ) str = 'Loading';
		$('body').append('<div id="blocker"></div>');
		$('body').append('<div id="ajax-loading"><img src="/template/gfx/loadingAnimation.gif" /><div><span id="message">'+str+'</span><span>...</span></div>');
		ezAjax.centerIt('#ajax-loading');
	},

	/*
	* Hide ajax loading and unblock UI
	*/

	hideLoading : function()
	{
		$('#blocker').remove();
		$('#ajax-loading').remove();
	},

	/*
	* Simple functions to manipulate checkboxes
	*/

	toggleXB : function(id)
	{
		$('#'+id)[0].checked = !$('#'+id)[0].checked;
	},
	
	toggleXBs : function(containerId,force)
	{
		$("#"+containerId+" input[@type='checkbox']").each(function() 
		{
			this.checked = force !== undefined ? force : !this.checked;
		});
	},
	
	/*
	* Check if enter key was hit
	*/
	
	isFormTrigger: function (e)
	{
		if ( ( (e.keyCode == 13 && e.shiftKey!= 1 )|| e.type === 'click' ) && ! ( $('#overlay').length > 0 || $('#blocker').length > 0 ) )
		{
			return true;
		}
	}

};

ezAjax.init();