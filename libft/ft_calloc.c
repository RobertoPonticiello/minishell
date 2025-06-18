/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ffebbrar <ffebbrar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 15:13:14 by ffebbrar          #+#    #+#             */
/*   Updated: 2024/01/15 18:58:46 by ffebbrar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t n, size_t size)
{
	size_t	total_size;
	void	*ptr;

	total_size = n * size;
	if (n + size < n || n + size < size || total_size > 2147483424)
		return (NULL);
	ptr = malloc(total_size);
	if (!ptr)
		return (NULL);
	while (total_size--)
		((char *)ptr)[total_size] = 0;
	return (ptr);
}
