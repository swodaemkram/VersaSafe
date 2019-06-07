/*
	Module: util.inc


*/


/**
 * load data into $input_vars
 *
 * @uses $input_vars
 * @return bool Success/failure
 */

function get_input_vars()
{
	global $input_vars;

	if (count($input_vars) == 0)
	{
		$input_vars = array();
	}

	if (! empty($_GET))
	{
		foreach ($_GET as $key => $value)
		{
			if (get_magic_quotes_gpc() == 1)
			{
				while (strstr($value, '\\'))
				{
					$value = stripslashes($value);
				}
			}
			$input_vars[$key] = $value;
		}
	}


	if (! empty($_POST))
	{
		foreach ($_POST as $key => $value)
		{
			if (get_magic_quotes_gpc() == 1)
			{
				while (strstr($value, '\\'))
				{
					$value = stripslashes($value);
				}
			}
			$input_vars[$key] = $value;
		}
	}


	if (empty($input_vars))
	{
		return false;
	} else {
		return true;
	}
}


