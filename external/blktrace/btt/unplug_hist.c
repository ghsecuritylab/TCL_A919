#include "globals.h"

#define BKT_WIDTH	5
#define MAX_BKT		19
#define EXCESS_BKT	20
#define NBKTS		(EXCESS_BKT + 1)

struct hist_bkt {
	__u32 dev;
	int hist[NBKTS * sizeof(int)];
};

void *unplug_hist_alloc(__u32 device)
{
	struct hist_bkt *hbp;

	if (unplug_hist_name == NULL) return NULL;

	hbp = malloc(sizeof(*hbp));
	hbp->dev = device;
	memset(hbp->hist, 0, NBKTS * sizeof(int));

	return hbp;
}

void unplug_hist_add(struct io *u_iop)
{
	struct d_info *dip;

	dip = __dip_find(u_iop->t.device);
	if (dip && dip->up_hist_handle) {
		__u64 *val = u_iop->pdu;
		int idx, n_unplugs = be64_to_cpu(*val);
		struct hist_bkt *hbp = dip->up_hist_handle;

		idx = (n_unplugs / BKT_WIDTH);
		if (idx > EXCESS_BKT)
			idx = EXCESS_BKT;

		hbp->hist[idx]++;
	}
}

void unplug_hist_free(void *arg)
{
	if (arg) {
		FILE *fp;
		struct hist_bkt *hbp = arg;
		int mjr = hbp->dev >> MINORBITS;
		int mnr = hbp->dev & ((1 << MINORBITS) - 1);
		char *oname = malloc(strlen(unplug_hist_name) + 32);

		sprintf(oname, "%s_%03d,%03d.dat", unplug_hist_name, mjr, mnr);
		if ((fp = my_fopen(oname, "w")) != NULL) {
			int i;

			for (i = 0; i < NBKTS; i++)
				fprintf(fp, "%d %d\n", i, hbp->hist[i]);
			fclose(fp);

		} else
			perror(oname);

		free(oname);
		free(hbp);
	}
}
