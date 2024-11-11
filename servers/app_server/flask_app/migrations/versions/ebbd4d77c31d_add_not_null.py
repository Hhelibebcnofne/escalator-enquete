"""Add Not Null

Revision ID: ebbd4d77c31d
Revises: 419315083c2a
Create Date: 2024-11-08 11:51:41.329107

"""
from alembic import op
import sqlalchemy as sa


# revision identifiers, used by Alembic.
revision = 'ebbd4d77c31d'
down_revision = '419315083c2a'
branch_labels = None
depends_on = None


def upgrade():
    # ### commands auto generated by Alembic - please adjust! ###
    with op.batch_alter_table('questions', schema=None) as batch_op:
        batch_op.alter_column('sentence',
               existing_type=sa.VARCHAR(length=255),
               nullable=False)
        batch_op.alter_column('optionA',
               existing_type=sa.VARCHAR(length=255),
               nullable=False)
        batch_op.alter_column('optionB',
               existing_type=sa.VARCHAR(length=255),
               nullable=False)

    with op.batch_alter_table('users', schema=None) as batch_op:
        batch_op.alter_column('name',
               existing_type=sa.VARCHAR(length=255),
               nullable=False)

    # ### end Alembic commands ###


def downgrade():
    # ### commands auto generated by Alembic - please adjust! ###
    with op.batch_alter_table('users', schema=None) as batch_op:
        batch_op.alter_column('name',
               existing_type=sa.VARCHAR(length=255),
               nullable=True)

    with op.batch_alter_table('questions', schema=None) as batch_op:
        batch_op.alter_column('optionB',
               existing_type=sa.VARCHAR(length=255),
               nullable=True)
        batch_op.alter_column('optionA',
               existing_type=sa.VARCHAR(length=255),
               nullable=True)
        batch_op.alter_column('sentence',
               existing_type=sa.VARCHAR(length=255),
               nullable=True)

    # ### end Alembic commands ###