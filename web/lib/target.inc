<?php

	class target
	{

		var $content;
		var $page_title    = APP_TITLE;
		var $content_title = APP_TITLE;
		var $swf_title     = APP_TITLE;
		var $action        = 'home';
		var $action_orig   = 'home';
		var $meta_desc     = '';
		var $meta_keys     = '';
		var $cache_id      = '';
		var $templates     = '';
		var $js_files      = false;
		var $css_files     = false;
		var $controller    = false;

		function target($action)
		{
			if ( $action )
			{
				$this->set_action($action);
			}
			
			$this->set_controller();
			$this->set_cache_id();
			$this->set_title();
			$this->set_templates();
			$this->set_js_files();
			$this->set_css_files();
		}

		function set_action($action)
		{

			$this->action_orig = $action;

			// Process incomming action
			$this->action = trim($action);

			// Tweak that allows use of tutorial and not article
			// (still stored in actions/articles tho)
			//$this->action = preg_replace('/^tutorial/','article',$this->action);

			// If there are sub nav options for this action
			// (i.e. bigups/youtube)
			$navcount=1;
			if ( preg_match('/\//',$this->action) )
			{
				$sub_navs = split('/',$this->action);
				$this->action = $sub_navs[0];
				//unset($sub_navs[0]);
				foreach( $sub_navs as $sub_nav )
				{
					$this->{"arg{$navcount}"} = $sub_nav;
					$navcount++;
				}
			}	
			
			$this->action        = strtolower($this->action);
			$this->action_root   = 'actions/'.$this->action;
			$this->web_root      = 'template';
			$this->gfx_root      = 'template/gfx';

		}

		function set_controller()
		{

			$possible_controller = str_replace('-','_',$this->action);

			// If there is a controller for this action, then let that file take care of it
			$controller_path = "$this->action_root/_{$possible_controller}_controller.proc";

			if ( file_exists($controller_path) )
			{
				$this->controller->root  = $possible_controller;
				$this->controller->path  = $controller_path;
				$this->controller->class = "{$possible_controller}_controller";
				$this->controller->view = $this->controller->method = "{$possible_controller}_index";

				if ( isset($this->arg2) )
				{
					$possible_method = str_replace('-','_',$this->arg2);
					$this->controller->view = $this->controller->method = "{$possible_controller}_{$possible_method}";
				}
			}
		}

		function set_title()
		{
			$this->page_title = APP_TITLE . ' - '.ucwords(str_replace('-',' ',strtolower($this->action)));
		}

		function set_cache_id()
		{
			global $APP_DOMAIN;
			$this->cache_id = "$APP_DOMAIN{$_SERVER['REQUEST_URI']}";
		}
		
		function set_templates()
		{
			$this->templates = array
			(
				'template/header.htm',
				'template/content.htm',
				'template/footer.htm',
			);
		}

		function set_js_files()
		{
			$this->js_files = array 
			(
				'/template/js/jquery.js', 
				'/template/js/ezAjax.js'
			);
		}

		/*
		* The path must NOT start with a slash
		*/
		function set_css_files()
		{
			$this->css_files = array 
			(
				'template/css/common.css'
			);
		}
		
	}

?>