/*
* Search
*/

ezAjax.register = 
{
	
	is_valid :
	{
		username : false,
		email : false,
		password : false,
		password2 : false
	},
	
	validate : function()
	{
		var error = false;
		
		if ( ! ezAjax.register.is_valid.username )
		{
			error = "Please enter a valid username";
		}
		else if ( ! ezAjax.register.is_valid.email )
		{
			error = "Please enter a valid email";
		}
		else if ( ! ezAjax.register.is_valid.password )
		{
			error = "Please enter a valid password (6 or more characters)";
		}
		else if ( ! ezAjax.register.is_valid.password2 )
		{
			error = "Please make sure that your password confirmation matches your password";
		}
		
		if ( error !== false )
		{
			alert(error);
			return false;
		}
		
		// It's validated!
		return true;
	},
	
	checkField : function(fieldObj)
	{
		eval('ezAjax.register.check_'+fieldObj.id)(fieldObj.value);
	},
	
	check_username : function(username)
	{
		
		ezAjax.register.is_valid.username = false;

		if ( username.length <= 1 )
		{
			$('.username span').html('');
			return;	
		}
		
		if ( username.length < 3 )
		{
			$('.username span').html('<img src="/template/gfx/icons/cross.png" />');
			return;	
		}

		$('.username span').html('<img src="/template/gfx/loading-small.gif" />');
		
		if ( username.match(/[^a-zA-Z0-9\_]/g, '') )
		{
			$('.username span').html('<span class="reg-error">Must be: a-zA-Z0-9_</span>');
			return;
		}
		
		ezAjax.ajax
		( 
			'register_check_username', 
			{
				username: username
			},
			function(response)
			{				
				if ( response.userAvailable )
				{
					ezAjax.register.is_valid.username = true;
					$('.username span').html('<img src="/template/gfx/icons/tick.png" />');
				}
				else
				{
					$('.username span').html('<span class="reg-error">Unavailable</span>');
				}
			},
			false,
			false
		);
	},
	
	check_email : function(email)
	{
		ezAjax.register.is_valid.email = false;

		if ( email.length <= 3 )
		{
			$('.email span').html('');
			return;	
		}
		
		if ( email.match(/[_a-z0-9\'-]+(\.[_a-z0-9\'-]+)*@[a-z0-9-]+(\.[a-z0-9-]+)*(\.[a-z]{2,4})$/g) )
		{
			
			ezAjax.ajax
			( 
				'register_check_username', 
				{
					username: email
				},
				function(response)
				{				
					if ( response.userAvailable )
					{
						ezAjax.register.is_valid.email = true;
						$('.email span').html('<img src="/template/gfx/icons/tick.png" />');
					}
					else
					{
						$('.email span').html('<span class="reg-error">Unavailable</span>');
					}
				},
				false,
				false
			);
			
		}
		else
		{
			$('.email span').html('<img src="/template/gfx/icons/cross.png" />');
		}
	},
	
	check_password : function(password)
	{
		ezAjax.register.is_valid.password = false;

		if ( password.length <= 3 )
		{
			$('.password span').html('');
			return;	
		}

		if ( password.length < 6 )
		{
			$('.password span').html('<img src="/template/gfx/icons/cross.png" />');
		}
		else
		{
			ezAjax.register.is_valid.password = true;
			$('.password span').html('<img src="/template/gfx/icons/tick.png" />');
		}

	},
	
	check_password2 : function(password2)
	{
		ezAjax.register.is_valid.password2 = false;

		if ( password2.length <= 3 )
		{
			$('.password2 span').html('');
			return;	
		}

		if ( password2 != $('#password').val() )
		{
			$('.password2 span').html('<img src="/template/gfx/icons/cross.png" />');
		}
		else
		{
			ezAjax.register.is_valid.password2 = true;
			$('.password2 span').html('<img src="/template/gfx/icons/tick.png" />');
		}
	}
};