/* This file is autogenerated by CAPO to support RPC */

#include "test1_purple_rpc.h"

void _notify_next_tag(gaps_tag* n_tag) {
    static int inited = 0;
    static void *psocket;
    static void *ssocket;
    gaps_tag t_tag;
    gaps_tag o_tag;
    #pragma cle begin TAG_NEXTRPC
    nextrpc_datatype nxt;
    #pragma cle end TAG_NEXTRPC
    #pragma cle begin TAG_OKAY
    okay_datatype okay;
    #pragma cle end TAG_OKAY

    nxt.mux = n_tag->mux;
    nxt.sec = n_tag->sec;
    nxt.typ = n_tag->typ;

    tag_write(&t_tag, MUX_NEXTRPC, SEC_NEXTRPC, DATA_TYP_NEXTRPC);
    tag_write(&o_tag, MUX_OKAY, SEC_OKAY, DATA_TYP_OKAY);

    if (!inited) {
      inited = 1;
      psocket = xdc_pub_socket();
      ssocket = xdc_sub_socket(o_tag);
      sleep(1); /* zmq socket join delay */
    }
  
    xdc_asyn_send(psocket, &nxt, &t_tag);
    xdc_blocking_recv(ssocket, &okay, &o_tag); 
    // XXX: check that we got valid OK?
}

double _rpc_get_a() {
    static int inited = 0;
    static void *psocket;
    static void *ssocket;
    gaps_tag t_tag;
    gaps_tag o_tag;
    #pragma cle begin TAG_REQUESTA
    requesta_datatype  reqA;
    #pragma cle end TAG_REQUESTA
    #pragma cle begin TAG_RESPONSEA
    responsea_datatype resA;
    #pragma cle end TAG_RESPONSEA

    reqA.x = 0;
    tag_write(&t_tag, MUX_REQUESTA, SEC_REQUESTA, DATA_TYP_REQUESTA);
    tag_write(&o_tag, MUX_RESPONSEA, SEC_RESPONSEA, DATA_TYP_RESPONSEA);

    if (!inited) {
      inited = 1;
      psocket = xdc_pub_socket();
      ssocket = xdc_sub_socket(o_tag);
      sleep(1); /* zmq socket join delay */
    }

    //_notify_next_tag(&t_tag);
    xdc_asyn_send(psocket, &reqA, &t_tag);
    xdc_blocking_recv(ssocket, &resA, &o_tag);

    return (resA.a);
}

void _hal_init(char *inuri, char *outuri)
{
  xdc_set_in(inuri);
  xdc_set_out(outuri);
  xdc_register(nextrpc_data_encode, nextrpc_data_decode, DATA_TYP_NEXTRPC);
  xdc_register(okay_data_encode, okay_data_decode, DATA_TYP_OKAY);
  xdc_register(requesta_data_encode, requesta_data_decode, DATA_TYP_REQUESTA);
  xdc_register(responsea_data_encode, responsea_data_decode, DATA_TYP_RESPONSEA);
}

void _master_rpc_init() {
    _hal_init((char*)INURI, (char *)OUTURI);
}
