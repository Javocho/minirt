/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akozin <akozin@student.42barcelona.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/18 15:48:10 by akozin            #+#    #+#             */
/*   Updated: 2024/07/24 16:42:20 by akozin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minirt.h"

static void	res_setter_internal(double temp, t_col *res, int i)
{
	if (!isinf(temp) && temp > 0 && (isinf(res->r_dist) || temp < res->r_dist))
	{
		res->r_dist = temp;
		res->obj_ind = i;
	}
}

/*
 * l - ray from the intersection point and to the... whatever, but
 * usually the light
 */
t_col	check_os_from_int_p(t_ray l, t_data *data, int i, t_col res)
{
	double	temp;

	if (data->objs[i].type == SP)
	{
		temp = sphere_intersection(l.o, l.f, &(data->objs[i]));
		res_setter_internal(temp, &res, i);
	}
	else if (data->objs[i].type == PL)
	{
		temp = splane_ray(l.o, l.f, data->objs[i].origin,
			data->objs[i].normal);
		res_setter_internal(temp, &res, i);
	}
	else if (data->objs[i].type == CY)
	{
		//temp = cyl_collide_check(); // TODO
		temp = -1;
		res_setter_internal(temp, &res, i);
	}
	return (res);
}

t_col	check_objs_internal(t_vec3 f, t_data *data, int i, t_col res)
{
	double	temp;

	if (data->objs[i].type == SP)
	{
		temp = sphere_intersection(data->cam.origin, f, &(data->objs[i]));
		res_setter_internal(temp, &res, i);
	}
	else if (data->objs[i].type == PL)
	{
		temp = splane_ray(data->cam.origin, f, data->objs[i].origin,
			data->objs[i].normal);
		res_setter_internal(temp, &res, i);
	}
	else if (data->objs[i].type == CY)
	{
		/*
		temp = cyl_collide_check(); // TODO
		*/
		temp = -1;
		res_setter_internal(temp, &res, i);
	}
	return (res);
}

static t_col	check_objects(t_vec3 f, t_data *data)
{
	int		i;
	t_col	res;

	i = 0;
	res.obj_ind = -1;
	res.r_dist = INFINITY;
	while (i < data->obj_n)
	{
		res = check_objs_internal(f, data, i, res);
		i++;
	}
	return (res);
}

void	draw(t_data *data)
{
	int		px;
	int		py;
	t_vec3	f;
	t_vec3	r;
	t_vec3	u;
	t_col	col;

	px = 0;
	r.x = 1;
	r.y = 0;
	r.z = 0;
	u.x = 0;
	u.y = -1;
	u.z = 0;
	while (px < WIN_X)
	{
//		printf("\npx: %d\n\n", px);
		py = 0;
		while (py < WIN_Y)
		{
			f = vec_add(data->cam.normal, vec_scale(r, WIN_X / WIN_Y *
				tan(data->cam.fov / 2.) * (2 * px / WIN_X - 1)));
			f = vec_add(f, vec_scale(u,
				tan(data->cam.fov / 2.) * (2 * py / WIN_Y - 1)));
			normalize(&f);
		//	printf("for pixel x: %2d, y: %2d we're launching ray: ", px, py);
		//	print_vector(f);
			col = check_objects(f, data);
			if (col.obj_ind != -1 && !isinf(col.r_dist))
				my_mlx_pixel_put(&data->img, px, py++,
					rgb_to_int(light_calc(data, col, f)));
			else
				my_mlx_pixel_put(&data->img, px, py++,
					rgb_to_int(rgb_scale(data->amb.color, data->amb.power)));
		}
		px++;
	}
	mlx_put_image_to_window(data->mlx, data->win, data->img.img, 0, 0);
}
/*
 * some notes:
 * 
 * amb light is probably the background color
 *
 * camera's normal vector is added to up and right vectors, multiplied
 * by corresponding coeffs to 1. go thru pixels 2. convert w/ the ratio
 * 3. take FOV into account. speaking of, we're gonna have the fov as vertical
 *
 * (imaginary img plane (= screen) things:)
 * screen height  = 2*h
 * screen width   = 2*w
 * used fov angle = given fov / 2
 *
 * considering the forward vector (cam's normal) is normalized,
 * h / 1 = tan (fov)
 * h = tan(fov)
 * w = h * ratio = h * WIN_X / WIN_Y = tan(fov) * WIN_X/WIN_Y
 *
 * representing a point on this imaginary plane with up and right vectors:
 * a*u + b*r = p
 * where a goes: -h..h
 * and   b       -w..w
 *
 * -w                    +w
 * _______________________ +h
 * |                     |
 * |         ^ u         |
 * |         |           |
 * |         . --> r     |
 * |                     |
 * |                     |
 * |_____________________| -h
 *
 * okay, so if we add this to our forward vector, we get the vector from the
 * camera's origin point through the specified point on the rendering plane:
 * f + h*y*u + w*x*r
 * here, x and y: -1..1
 *
 * so, coming back to the WIN_X WIN_Y values for the actual pixels we'll use,
 * we need to convert x from 0..WIN_X to -1..1, and y has a similar story
 * px: 0..WIN_X
 * x: -1..1
 * => x = 2 * px / WIN_X - 1
 * so, for y,
 * => y = 2 * py / WIN_Y - 1
 *
 * so for our scene, x is to the right, y is up, and z is depth.
 *
 * this should be enough.
 */
