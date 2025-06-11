/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_p.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ffebbrar <ffebbrar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/24 14:02:08 by ffebbrar          #+#    #+#             */
/*   Updated: 2024/01/29 15:25:56 by ffebbrar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static int	ft_countpointer(unsigned long long prt)
{
	int	count;

	count = 0;
	while (prt != 0)
	{
		count++;
		prt = prt / 16;
	}
	return (count);
}

int	ft_printpointer(unsigned long long prt)
{
	if (prt >= 16)
	{
		ft_printpointer(prt / 16);
		ft_printpointer(prt % 16);
	}
	else
	{
		if (prt <= 9)
			ft_putchar(prt + 48);
		else
			ft_putchar(prt - 10 + 'a');
	}
	return (ft_countpointer(prt));
}

int	ft_putpointer(unsigned long long ptr)
{
	int		len;

	len = 0;
	if (ptr == 0)
	{
		len += write (1, "0x0", 3);
		return (len);
	}
	len = write (1, "0x", 2);
	len += ft_printpointer(ptr);
	return (len);
}