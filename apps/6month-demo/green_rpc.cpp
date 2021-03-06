#include <iostream>

#include "ownship.h"

#include "hal_xdcomms.h"
#include "hal_gma.h"

void OwnShipShadow::receive()
{
    #pragma cle begin TAG_2_2_1
    gaps_tag t_tag;
    #pragma cle end TAG_2_2_1
    uint32_t t_mux = 2, t_sec = 2, type = DATA_TYP_POSITION;

    tag_write(&t_tag, t_mux, t_sec, type);
    void *socket = xdc_sub_socket(t_tag);

    Position position(0, 0, 0);
    position_datatype pos;

    while (1) {
        xdc_blocking_recv(socket, &pos, &t_tag);
        position._x = pos.x;
        position._y = pos.y;
        position._z = pos.z;

        setPosition(position);
        notify();
    }
    zmq_close(socket);
}

void RfSensorShadow::receive()
{
    #pragma cle begin TAG_2_2_2
    gaps_tag t_tag;
    #pragma cle end TAG_2_2_2
    uint32_t t_mux = 2, t_sec = 2, type = DATA_TYP_DISTANCE;

    tag_write(&t_tag, t_mux, t_sec, type);
    void *socket = xdc_sub_socket(t_tag);

    Distance distance(0, 0, 0);
    distance_datatype dis;

    while (1) {
        xdc_blocking_recv(socket, &dis, &t_tag);
        distance._dx = dis.x;
        distance._dy = dis.y;
        distance._dz = dis.z;

        setDistance(distance);
        notify();
    }
    zmq_close(socket);
}

void *hal_init()
{
  void *ctx = xdc_ctx();
  xdc_set_out((char *)"ipc:///tmp/halpubbwgreen");
  xdc_set_in((char *)"ipc:///tmp/halsubbwgreen");
    
  xdc_register(position_data_encode, position_data_decode, DATA_TYP_POSITION);
  xdc_register(distance_data_encode, distance_data_decode, DATA_TYP_DISTANCE);

  return ctx;
}
