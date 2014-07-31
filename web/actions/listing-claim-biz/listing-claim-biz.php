<?
	include "interfaces/biz/biz_details.inc";

	$business = new business;
	$biz_logged_in = false;

	$__biz = $business->get($TARGET->arg3);

	if ( session_is_active())
	{
	 	if (session_is_member() )
		{
			$text="You are logged in with a member account and are attempting to claim Complete Family Fitness & Aerobics of Dora Alabama. In order to claim a listing, you must be logged in with a business account. Please follow the instructions below to get started:";
		}
		else if (session_is_biz_owner())
		{
			include "interfaces/biz/biz_link_to_user.inc";
			$biz_link_to_user = new biz_link_to_user;

			if ($biz_link_to_user->is_requested(myuid(),$TARGET->arg3))
			{
				notify_redirect("/biz/listings");
			}
			else if (isset($_GET['claim']) && isset($_GET['claim']) == 'true')
			{
				$biz_link_to_user->request(myuid(),$TARGET->arg3);
				notify_redirect(get_biz_link($__biz),'We have received your claim request. Please allow 4 hours for your claim to be approved.');
			}
			$biz_logged_in = true;

			$text = "To claim this listing, please click the claim listing button below. Claims are manually<br>verified by our staff for security purposes.";
		}
	}
	else
	{
		include "lib/state_utils.inc";
		$__biz->state = get_long_state($__biz->state);
		$text = 'To claim '."<b>{$__biz->name}</b> of {$__biz->city}, {$__biz->state}".' please follow the instructions below.<br>Claims are manually verified by our staff for security purposes.';
		$_SESSION['RESTRICTED_ACTION'] = $_SERVER['REQUEST_URI'];
	}
	
?>
