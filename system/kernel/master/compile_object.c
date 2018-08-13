/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : compile_object.c
 * Author : Clode@RevivalWorld
 * Date   : 2001-07-05
 * Note   : included by Master Object
 * Update :
 *  o 2002-00-00

 -----------------------------------------
 */

// compile_object: This is used for loading MudOS "virtual" objects.
// It should return the object the mudlib wishes to associate with the
// filename named by 'file'.  It should return 0 if no object is to be
// associated.
/*
�W�١G
        compile_object - ���ѵ������� (virtual object) �\�઺ mudlib �����C
�y�k�G
        object compile_object( string pathname );

        ���� compile_object( �r�� ���|�W );
�Ϊk�G
        �� mudlib �����X�ʵ{�����J�@�Ӥ��s�b��O���餤���ɮסA�X�ʵ{���|�I
        �s compile_object �禡�C�|�Ҩӻ��A�p�G mudlib �I�s
        call_other("/obj/file.r", "some_function") �� new("/obj/file.r") ��
        �A�O���餤�S���s�� /obj/file.r.c ���F��A�X�ʵ{���N�I�s�D�_����
         (master object) �� compile_object("/obj/file.r")�C�p�G mudlib ����
        ��s�� (associate) /obj/file.r �ɮסAcompile_object() �N�Ǧ^ 0�C�p
        �G mudlib �@�N�A�N�n�Ǧ^ /obj/file.r �o�Ӫ���C�b compile_object()
        �s���@���ɮצW�٩M���󤧫�A���X�ʵ{���Ө��A�N�p�P�C�����s�b
        file.r.c �o���ɮסA�Ӹ��J������ɲ��ͪ��N�O compile_object() �Ǧ^��
        ����C
�@�̡G
        Tim Hollebeek  Beek@ZorkMUD, Lima Bean, IdeaExchange, and elsewhere
½Ķ�G
        spock@muds.net          2000.May.20.    v22 ��
*/

#include <daemon.h>

varargs mixed compile_object(string file, int cloning)
{
	return VIRTUAL_D->compile_object(file, cloning);
}