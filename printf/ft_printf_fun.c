/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_fun.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ffebbrar <ffebbrar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/23 14:36:18 by ffebbrar          #+#    #+#             */
/*   Updated: 2024/01/29 15:08:14 by ffebbrar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_putchar(char c)
{
	write(1, &c, 1);
	return (1);
}

int	ft_putnumb(int n)
{
	int	i;

	i = 0;
	if (n == -2147483648)
	{
		ft_putchar('-');
		ft_putchar('2');
		ft_putnumb(147483648);
		return (11);
	}
	else if (n < 0)
	{
		i += ft_putchar('-');
		i += ft_putnumb(-n);
	}
	else if (n >= 10)
	{
		i += ft_putnumb(n / 10);
		i += ft_putnumb(n % 10);
	}
	else
		i += ft_putchar(n + 48);
	return (i);
}

size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

int	ft_putstr(char *str)
{
	int	len;

	if (!str)
	{
		write (1, "(null)", 6);
		return (6);
	}
	len = ft_strlen(str);
	write (1, str, len);
	return (len);
}