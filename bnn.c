#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#ifndef LABELS_MAX
# define LABELS_MAX 255
#endif
#ifndef INPUTS_MAX
# define INPUTS_MAX 255
#endif
#ifndef CELLS_MAX
# define CELLS_MAX 255
#endif
#ifndef OUTPUT_MAX
# define OUTPUT_MAX 255
#endif
#ifndef DEBUG
# define DEBUG 1
#endif
typedef char	t_bin;

typedef struct 		s_bgraph
{
	char			*label;
	struct s_bgraph	*inputs[2];
	t_bin			f[4];
}					t_bgraph;
// this is a chromosome
typedef struct		s_bnn
{
	t_bgraph		inputs[INPUTS_MAX];
	t_bgraph		cells[CELLS_MAX];
	char			*outputs[OUTPUT_MAX];
	size_t			input_count;
	size_t			cell_count;
	size_t			output_count;
	size_t			generation;
}	t_bnn;

t_bin				doop(t_bin f[4], t_bin a, t_bin b)
{
	if (!a && !b)
		return f[0];
	else if (!a && b)
		return f[1];
	else if (a && !b)
		return f[2];
	return f[3];
}

t_bgraph			*find_cell(char *label, t_bnn bnn)
{
	size_t			i;
	void			*r;

	i = 0;
	while (i < bnn.input_count)
	{
		if (!strcmp(label, bnn.inputs[i].label))
		{
			r = bnn.inputs+ i;
			return (r);
		}
		i += 1;
	}
	i = 0;
	while (i < bnn.cell_count)
	{
		if (!strcmp(label, bnn.cells[i].label))
		{
			r = bnn.cells+ i;
			return (r);
		}
		i += 1;
	}
	return (0);
}

t_bin				dogop(t_bgraph *cell)
{
	if (!cell->inputs[0])
		return (cell->f[0]);
	return (doop(cell->f, dogop(cell->inputs[0]), dogop(cell->inputs[1])));
}

t_bin				bnn_run(t_bnn bnn, char *s, char *output)
{
	size_t	i;
	t_bin	r;

	printf("bnn_run(%s)\n", s);
	i = 0;
	while (s[i])
	{
		bnn.inputs[i].f[0] = s[i];
		i += 1;
	}
	i = 0;
	while (i < bnn.output_count)
	{
		if (!strcmp(bnn.outputs[i], output))
		{
			return dogop( find_cell(bnn.outputs[i], bnn) ); 
		}
		i += 1;
	}
	printf("error - unable to find output label %s\n", output);
	return 0;
}

void				bnn_mutate(t_bnn *bnn)
{
	size_t	cell;
	int		bit;

	cell = rand() % bnn->cell_count;
	bit = rand() % 4;
	bnn->generation += 1;
	printf("toggeling cell %zu bit %i\n", cell, bit);
	bnn->cells[cell].f[bit] = bnn->cells[cell].f[bit] == '0' ? '1' : '0';
}

void				bnn_write(char *path, t_bnn bnn)
{
	int	fd;
	size_t	i;

	fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	i = 0;
	while (i < bnn.input_count)
	{
		write(fd, bnn.inputs[i].label, strlen(bnn.inputs[i].label));
		i += 1;
		if (i != bnn.input_count)	
			write(fd, ", ", 2);
		else
			write(fd, ".\n", 2);
		i += 1;
	}
	i = 0;
	while (i < bnn.cell_count)
	{
		write(fd, bnn.cells[i].label, strlen(bnn.cells[i].label));
		write(fd, "\t= ", 3);
		write(fd, bnn.cells[i].f, 4);
		write(fd, "(", 1);
		write(fd, ")\n", 2);
		i += 1;
	}
	close(fd);
}

int 				main(int ac, char **av)
{
	int				fd;
	char			c;
	char			label[LABELS_MAX];
	t_bnn			bnn;
	t_bgraph		cell;
	size_t			i;
	int				init;
	int				wait_f;
	int				wait_output;
	if (ac != 2)
		return !!printf("usage: %s source.bnn\n", av[0]);
#if DEBUG == 0
	srand(time(0));
#endif
	fd = open(av[1], O_RDONLY);
	init = 0;
	i = 0;
	bnn.input_count = 0;
	wait_f = 0;
	bnn.cell_count = 0;
	wait_output = 0;
	bnn.output_count = 0;
	bnn.generation = 0;
	while (read(fd, &c, 1) > 0)
	{
		switch (c)
		{
			case '.':
			case ',':
				if (!init)
				{
					printf("inputs[%zu](%p) = %s\n", bnn.input_count, bnn.inputs + bnn.input_count, 
							label);
					bnn.inputs[bnn.input_count++] = (t_bgraph) 
					{
						strdup(label), 
						{0,0}, 
						{0,0,0,0}
					};
					i = 0;
				}
				else
				{
					i = 0;
					cell.inputs[0] = find_cell(label, bnn);
					if (!cell.inputs[0])
					{
						printf("error: unable to find %s!\n", label);
					}
				}
				if (c == '.')
					init = 1;
				break;
			case '_':
				wait_output = 1;
				break;
			case '=':
				if (find_cell(label, bnn))
				{
					printf("error - label %s already in use\n", label);
				}
				if (wait_output)
					bnn.outputs[bnn.output_count++] = strdup(label);
				cell.label = strdup(label);
				i = 0;
				wait_f = 1;
				break ;
			case '\t':
			case ' ':
			case '\n':
				break ;
			case '(':	
				wait_f = 0;
				break ;
			case ')':
				wait_f = 0;
				cell.inputs[1] = find_cell(label, bnn);
				if (!cell.inputs[1])
				{
					printf("error: unable to find %s!\n", label);
				}

				bnn.cells[bnn.cell_count] = cell;
				printf("cells[%zu](%p)(%s) => {%p, %p}[%c%c%c%c]\n", 
						bnn.cell_count, bnn.cell_count + bnn.cells, cell.label,
						cell.inputs[0], cell.inputs[1], cell.f[0], cell.f[1], cell.f[2], cell.f[3]);
				bnn.cell_count += 1;
				i = 0;
				break;
			default:
				if (wait_f)
				{
					cell.f[wait_f++ - 1] = c;
				}
				else
				{
					label[i++] = c;
					label[i] = 0;
				}
				break;
		}
	}
	/**
	 full adder truth table

		000	00
		001	10
		010	10
		011	01
		100	10
		101	01
		110	01
		111	11
*/
	printf("training...\n");
	int	success;
	t_bnn bnn2;
	success = 0;
	while (!success)
	{
		bnn.generation += 1;
		printf("generation #%zu\n", bnn.generation);
		bnn2 = bnn;
		// TODO: non-expressive genes + crossover
		bnn_mutate(&bnn2);
		success = 1;
		if (bnn_run(bnn2, "110", "R1") != '0')
			success = 0;
		if (bnn_run(bnn2, "111", "R1") != '1')
			success = 0;
		if (bnn_run(bnn, "000", "R1") != '0')
			success = 0;
	}

	printf("testing...\n");
	bnn_run(bnn, "111", "R1");
	bnn_run(bnn, "000", "R1");

	bnn_write("demo.trained.bnn", bnn);
}
