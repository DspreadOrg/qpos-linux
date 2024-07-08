#include "ui_MultiApp.h"

struct App
{
    u8 counter;
    char listName[5][20];
};

static struct App LocalMultiApp;
static pu8 LocalInputTlv = NULL;

static void UpdateKernelSelected(u8 selected)
{
    u8 pNode[32] = {0};
    tlv_new_(pNode, POS_TAG_HEAD, 0, NULL);

    tlv_add_child_(pNode, KERNEL_TAG_AID_SELECT_RESULT_INDEX, 1, &selected);
    tlv_replace_(LocalInputTlv, POS_TAG_RES_KN_APP_DATA, tlv_get_node_l_(pNode), pNode);
}

static u8 unpackCounter(pu8 tlvAppData)
{
    pu8 counterNode = tlv_find_(tlvAppData, KERNEL_TAG_AID_LIST_CANDIDATE_COUNTER);

    if (!counterNode || !tlv_get_l_(counterNode))
    {
        return 0;
    }
    pu8 value = NULL;
    value = tlv_get_v_(counterNode);
    u8 counter = value[0];

    return counter;
}

static int unpackAppsName(u8 counter, pu8 tlvAppData)
{
    memset(&LocalMultiApp, 0x00, sizeof(LocalMultiApp));
    LocalMultiApp.counter = counter;

    pu8 pNode = tlv_find_(tlvAppData, KERNEL_TAG_AID_LIST_DISPLAYNAME);

    pu8 pChild=NULL;

    pChild = tlv_get_first_child_(tlvAppData);

    for (int index = 0; index < counter; index++)
    {
        if (pChild)
        {
            if (tlv_get_t_(pChild) == KERNEL_TAG_AID_LIST_DISPLAYNAME)
            {
                Memcpy_(LocalMultiApp.listName[index], (char *)tlv_get_v_(pChild), tlv_get_l_(pChild));                
            }
            else
            {
                return ERROR_CODE;
            }
        }
        pChild = tlv_get_next_child_(pChild);
    }

    return 1;
}

bool unpackAppData(pu8 tlvData)
{
    T_U8_VIEW tlvAppData = get_tlv_view(tlvData, POS_TAG_RES_KN_APP_DATA);

    if (!UV_OK(tlvAppData))
    {
        return false;
    }

    return unpackAppsName(unpackCounter(tlvAppData.head), tlvAppData.head) > 0;
}

void selectMultiApp(pu8 tlvData)
{
    LocalInputTlv = tlvData;
    int nIndexOfSelect = 0;
    int ret = 0;
    if (!unpackAppData(tlvData))
    {
        ;
    }
    else
    {
        char * dispList[5];
        for (int i = 0; i <LocalMultiApp.counter; i++) {
            dispList[i] = LocalMultiApp.listName[i];
        }
        ret = TransView_nShowListViewEx((char *)"Plz select APP", dispList, LocalMultiApp.counter, 60, &nIndexOfSelect);
        if (RET_DPORT_NORMAL == ret)
        {
            UpdateKernelSelected((u8)nIndexOfSelect);
        }    
    }
}
