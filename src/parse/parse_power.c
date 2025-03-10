/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_power.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akozin <akozin@student.42barcelona.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/16 15:21:27 by akozin            #+#    #+#             */
/*   Updated: 2024/07/17 16:31:25 by akozin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minirt.h"

double	atod_res_add_logic(int *ptp, double res, char c)
{
	if (!*ptp)
		res = res * 10. + (double)(c - '0');
	else
		res += ((double)(c - '0')) / pow(10., (*ptp)++);
	return (res);
}

double	parse_power(t_data *data, char *s)
{
	int		past_the_point;
	double	res;

	past_the_point = 0;
	res = 0.0;
	while (*s && *s != '\n')
	{
		if (*s == '-')
			return (data->error = DOUBLE_ERR, -0.0);
		if (ft_isdigit(*s))
			res = atod_res_add_logic(&past_the_point, res, *s);
		else if (*s == '.' && !past_the_point)
			past_the_point = 1;
		else
			return (data->error = DOUBLE_ERR, -0.0);
		s++;
	}
	if (res > 1.0)
		return (data->error = DOUBLE_ERR, -0.0);
	return (res);
}
