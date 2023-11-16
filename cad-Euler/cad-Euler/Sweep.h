#pragma once
#include "EularOp.h"
#include <vector>
#include<iostream>
using namespace std;

void sweep(Face* f, float direction[], std::vector<Vertex*>& new_v, Face* outter_face) {
    // new_v Ϊɨ�ɲ����´����ĵ㣬�����Ա����ģ�͵�
    f = f->fsolid->sface;
    std::vector<Face*> face_list;
    for (Face* tmp_f = f; f != nullptr; f = f->nextf) {
        face_list.push_back(f);
    }
    if (face_list.size() == 2) {
        face_list.clear();
        face_list.push_back(outter_face);
    }

    for (int i = 0; i < face_list.size();i++) {
        Face* tmp_f = face_list[i];
        if (tmp_f->floop->nextl != nullptr) {
            continue;
        }
        int v_beidx = new_v.size();
        Loop* lp = tmp_f->floop;//����f1��lp2
        HalfEdge* he = lp->ledg;
        Vertex* firstv = he->startv;

        Face* newf;
        Edge* newe;
        Vertex* newv;
        {
            //ÿһ��֪�㣬ͨ�������µļ��ϵ㣬����һ��ֱ�߱�
            float new_pos[3] = { he->startv->x + direction[0], he->startv->y + direction[1] ,he->startv->z + direction[2] };
            mev(&newe, &newv, lp, he->startv, new_pos);
        }
        new_v.push_back(newv);
        he = he->next;
        while (he->startv != firstv) {
            float new_pos[3] = { he->startv->x + direction[0], he->startv->y + direction[1] ,he->startv->z + direction[2] };
            mev(&newe, &newv, lp, he->startv, new_pos);
            new_v.push_back(newv);
            he = he->next;
        }
        // ������
        for (int j = v_beidx; j < new_v.size() - 1; j++) {
            mef(&newe, &newf, new_v[j], new_v[j + 1], lp);
        }

        mef(&newe, &newf, new_v[new_v.size() - 1], new_v[v_beidx], lp);
    }

    for (int i = 0; i < face_list.size(); i++) {
        Face* tmp_f = face_list[i];
        //����ͨ��
        if (tmp_f->floop->nextl == nullptr && tmp_f != outter_face) {
            kfmrh(outter_face, tmp_f);
        }
    }
}

