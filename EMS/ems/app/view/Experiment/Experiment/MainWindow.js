/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Andrey Kartashov .
 ** All rights reserved.
 ** Contact: Andrey Kartashov (porter@porter.st)
 **
 ** This file is part of the EMS web interface module of the genome-tools.
 **
 ** GNU Lesser General Public License Usage
 ** This file may be used under the terms of the GNU Lesser General Public
 ** License version 2.1 as published by the Free Software Foundation and
 ** appearing in the file LICENSE.LGPL included in the packaging of this
 ** file. Please review the following information to ensure the GNU Lesser
 ** General Public License version 2.1 requirements will be met:
 ** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 **
 ** Other Usage
 ** Alternatively, this file may be used in accordance with the terms and
 ** conditions contained in a signed written agreement between you and Andrey Kartashov.
 **
 ****************************************************************************/


Ext.define('EMS.view.Experiment.Experiment.MainWindow', {
    extend: 'Ext.window.Window',
    alias: 'widget.experimentmainwindow',

    title: 'Experiment review',
    layout: 'fit',
    iconCls: 'table2',
    buttonAlign: 'center',
    maximizable: true,
    //constrain: true,

    plain: true,
    height: 700,
    width: 1000,
    border: false,
    focusOnToFront: true,
    UID: "",
    requires: [
        'EMS.view.Experiment.Experiment.EditForm',
    ],

    items: [
        {
            xtype: 'tabpanel',
            itemId: 'experimentmainwindowtabpanel',
            frame: false,
            border: false,
            plain: true,
            activeTab: 0,
            items: [
                {
                    xtype: 'experimenteditform',
                    frame: false,
                    UID: this.UID,
                    plain: true,
                    border: false,
                    title: 'Experiment form',
                    layout: 'fit',
                    iconCls: 'form-blue-edit'
                }
            ]
        }
    ],

    buttons: [
        {
            text: 'Save',
            itemId: 'save'
        } ,
        {
            text: 'Cancel',
            itemId: 'cancel'
        }
    ]
});

