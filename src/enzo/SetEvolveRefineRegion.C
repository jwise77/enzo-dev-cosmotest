/*------------------------------------------------------------------------
  SET REFINE REGION FROM EVOLVING REGION
  By John Wise

  History:
     03 May 2005 : JHW -- Created
------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include "macros_and_parameters.h"
#include "typedefs.h"
#include "global_data.h"
#include "CosmologyParameters.h"

int CosmologyComputeExpansionFactor(FLOAT time, FLOAT *a, FLOAT *dadt);

int SetEvolveRefineRegion (FLOAT time) 
{

  int timestep, staticRegion, i;
  FLOAT a, dadt, redshift;

  /* Return if not used */

  if (EvolveRefineRegionTime[0] <= FLOAT_UNDEFINED)
    return SUCCESS;

  /* If TimeType is redshift, calculate redshift */

  if (ComovingCoordinates) {
    CosmologyComputeExpansionFactor(time, &a, &dadt);
    redshift = (1 + InitialRedshift)/a - 1;
  }

  /* Find closest time step with <time */
  /* Set time=redshift if that's what we're doing. */
  if (RefineRegionTimeType == 1) {
    time = redshift;
  }
  timestep = 0;
  while (timestep < MAX_REFINE_REGIONS && 
	 EvolveRefineRegionTime[timestep] >= FLOAT_UNDEFINED &&
	 time < EvolveRefineRegionTime[timestep])
    timestep++;
  timestep -= 1;
  if (timestep < 0) return SUCCESS;

  /* Set RefineRegion to EvolveRefineRegion or innermost StaticRefineRegion */

  staticRegion = 0;
  while (StaticRefineRegionLevel[staticRegion] != INT_UNDEFINED)
    staticRegion++;
  staticRegion -= 1;

  for (i = 0; i < MAX_DIMENSION; i++)
    if (staticRegion < 0) {
      RefineRegionLeftEdge[i] = EvolveRefineRegionLeftEdge[timestep][i];
      RefineRegionRightEdge[i] = EvolveRefineRegionRightEdge[timestep][i];
    } else {
      RefineRegionLeftEdge[i] = max(EvolveRefineRegionLeftEdge[timestep][i],
				    StaticRefineRegionLeftEdge[staticRegion][i]);
      RefineRegionRightEdge[i] = 
	min(EvolveRefineRegionRightEdge[timestep][i],
	    StaticRefineRegionRightEdge[staticRegion][i]);
    }

  if (debug)
    fprintf(stdout, "EvolveRegion: %"FSYM" %"FSYM" %"FSYM" %"FSYM" %"FSYM" %"FSYM"\n",
	    RefineRegionLeftEdge[0], RefineRegionLeftEdge[1], 
	    RefineRegionLeftEdge[2], RefineRegionRightEdge[0],
	    RefineRegionRightEdge[1], RefineRegionRightEdge[2]);

  return SUCCESS;

}
