<?php

	/**
	 * Clever little function that takes a table structure
	 * defined in PHP, and turns into a mySQL table, and creates
	 * it in the db.
	 */

	function create_table(&$table_structure)
	{
		global $db;
		
		$sql = '';
		foreach ( $table_structure as $type => $fields )
		{
			
			if ( ! is_array($fields) ) continue;	
			
			foreach ($fields as $key => $field)
			{
				switch ($type)
				{
					case 'INDEX':
						$sql .= "INDEX {$field}_idx ($field),\n";
						break;

					case 'FULLTEXT':
						$sql .= "FULLTEXT ($field),\n";
						break;

					case 'KEY':
						$sql .= "KEY ($field),\n";
						break;
						
					case 'PRIMARY KEY':
						$sql .= "PRIMARY KEY ($field),\n";
						break;

					case 'ENUM':
						
						$sql .= "$key ENUM(";
						
						$enum_vals = '';
						foreach ( $field['values'] as $enum_val )
						{
							$enum_vals .= "'$enum_val',";
						}
						
						$sql .= substr($enum_vals,0,-1).')';
						
						if ( $field['default'] )
						{
							$sql .= " DEFAULT '{$field['default']}'";
						}
						
						$sql .= ",\n";
					
						break;

					default:
						$sql .= "$field $type,\n";
				}
			}
		}
		
		$name = $table_structure['table_name'];
		$sql = rtrim($sql,",\n");

		/**
		 * Execute the SQL to make this thing happen
		 */

		$db->query("DROP TABLE IF EXISTS $name");
		
		$db->query("CREATE TABLE $name ( $sql ) TYPE=MyISAM /*!40100 DEFAULT CHARACTER SET utf8 */");
	}

?>