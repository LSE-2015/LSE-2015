#include <stdlib.h>
#include <stdio.h>
#include "serial.h"
#include "comNodoCentral.h"

void main()
{
   int i;
   int k;
   beacon_ID_t beacon_ID;
   drone_ID_t drone_ID;
   beacon_element_t *beacon_element;
   beacons_table_t beacons_table;
   
   beacons_table = create_beacons_table();
   //5 beacons registered
   for (i = 1; i <= 4; i++)
   {
     beacons_table.elements[beacons_table.reg_elements].beacon = i;
     beacons_table.reg_elements++;
   }
  
   //Beacon 1 . Drones: 1,2,3,4,5,6,7,8
   beacon_ID = (uint16_t) 1;
   for (k = 0; k <= 7; k++)
   {
        drone_ID = k + 1;
        beacon_element = find_beacon (beacon_ID, &beacons_table);
        //beacon_element = 0x7FFFFFFFFFFF & find_beacon (beacon_ID, &beacons_table);
        //printf("%p\n\r",&beacons_table);
        //printf("%p\n\r",beacon_element);
        
        if (beacon_element != NULL) //The beacon was registered
        {
           for (i = 0; i <= beacon_element->reg_drones; i++)
           {
              if(beacon_element->drones_table[i] == drone_ID)
              {
                 break; //It is registered, do nothing
              }
           }
           beacon_element->drones_table[beacon_element->reg_drones] = drone_ID;
           beacon_element->reg_drones++;
        }
    }

   //Beacon 2 . Drones: 2,4,6,8
   beacon_ID = (uint16_t) 2;
   for (k = 0; k <= 3; k++)
   {
        drone_ID = 2*(k + 1);
        beacon_element = find_beacon (beacon_ID, &beacons_table);
        if (beacon_element != NULL) //The beacon was registered
        {
           for (i = 0; i <= beacon_element->reg_drones; i++)
           {
              if(beacon_element->drones_table[i] == drone_ID)
              {
                 break; //It is registered, do nothing
              }
           }
           beacon_element->drones_table[beacon_element->reg_drones] = drone_ID;
           beacon_element->reg_drones++;
        } 
    }

   //Beacon 3 . Drones: 1,3,5,7,9
   beacon_ID = (uint16_t) 3;
   for (k = 0; k <= 4; k++)
   {
        drone_ID = 2*k + 1;
        beacon_element = find_beacon (beacon_ID, &beacons_table);
        if (beacon_element != NULL) //The beacon was registered
        {
           for (i = 0; i <= beacon_element->reg_drones; i++)
           {
              if(beacon_element->drones_table[i] == drone_ID)
              {
                 break; //It is registered, do nothing
              }
           }
           beacon_element->drones_table[beacon_element->reg_drones] = drone_ID;
           beacon_element->reg_drones++;
        } 
    }

   //Beacon 4 . Drones: 5,7,9
   beacon_ID = (uint16_t) 4;
   for (k = 0; k <= 2; k++)
   {
        drone_ID = 2*(k + 2) + 1;
        beacon_element = find_beacon (beacon_ID, &beacons_table);
        if (beacon_element != NULL) //The beacon was registered
        {
           for (i = 0; i <= beacon_element->reg_drones; i++)
           {
              if(beacon_element->drones_table[i] == drone_ID)
              {
                 break; //It is registered, do nothing
              }
           }
           beacon_element->drones_table[beacon_element->reg_drones] = drone_ID;
           beacon_element->reg_drones++;
        } 
    }

   print_beacon_table(beacons_table);

   return;
}
