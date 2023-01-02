#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import shlex

# #############################################################################
# Get version of the package from the package itself.
# #############################################################################

project = 'iteration_utilities'


def get_version():
    with open('../src/{}/__init__.py'.format(project)) as f:
        for line in f:
            if line.startswith('__version__'):
                return line.split(r"'")[1]


# #############################################################################
# Custom stuff
# #############################################################################

author = 'Michael Seifert'
project_description = 'Utilities based on Pythons iterators and generators.'
project_category = 'Miscellaneous'
project_startyear = '2016'


# #############################################################################
# Official options
# #############################################################################

# sys.path.insert(0, os.path.abspath('.'))

# -- General configuration ----------------------------------------------------
extensions = ['sphinx.ext.autodoc',
              'sphinx.ext.intersphinx',
              'sphinx.ext.mathjax',
              'numpydoc',
              ]
source_suffix = '.rst'
# source_encoding
# source_parsers
master_doc = 'index'
exclude_patterns = ['_build', '_templates']
templates_path = ['_templates']
# template_bridge
# rst_epilog
# rst_prolog
# primary_domain
# default_role
# keep_warnings
# suppress_warnings
# needs_sphinx = '1.0'
# needs_extensions
nitpicky = True
# nitpick_ignore
# numfig
# numfig_format
# numfig_secnum_depth
# tls_verify
# tls_cacerts

# -- Project information ------------------------------------------------------

project = 'iteration_utilities'
copyright = project_startyear + ', ' + author
version = get_version()
release = get_version()
# today
# today_fmt
# highlight_language
# highlight_options
pygments_style = 'sphinx'
# add_function_parentheses
# add_module_names
# show_authors
# modindex_common_prefix
# trim_footnote_reference_space
# trim_doctest_flags

# -- Options for internationalization -----------------------------------------

language = 'en'
# locale_dirs
# gettext_compact
# gettext_uuid
# gettext_location
# gettext_auto_build
# gettext_additional_targets
# figure_language_filename

# -- Options for HTML output --------------------------------------------------

html_theme = 'nature'
# html_theme_options
# html_theme_path
# html_style
# html_title
# html_short_title
# html_context
# html_logo
# html_favicon
# html_static_path = ['_static']
# html_extra_path
# html_last_updated_fmt
# html_use_smartypants
# html_add_permalinks
# html_sidebars
# html_additional_pages
# html_domain_indices
# html_use_modindex
# html_use_index
# html_split_index
# html_copy_source
# html_show_sourcelink
# html_sourcelink_suffix
# html_use_opensearch
# html_file_suffix
# html_link_suffix
# html_translator_class
# html_show_copyright
# html_show_sphinx
# html_output_encoding
# html_compact_lists
# html_secnumber_suffix
# html_search_language
# html_search_options
# html_search_scorer
# html_scaled_image_link
htmlhelp_basename = project + 'doc'

# -- Options for Apple Help output --------------------------------------------

# applehelp_bundle_name
# applehelp_bundle_id
# applehelp_dev_region
# applehelp_bundle_version
# applehelp_icon
# applehelp_kb_product
# applehelp_kb_url
# applehelp_remote_url
# applehelp_index_anchors
# applehelp_min_term_length
# applehelp_stopwords
# applehelp_locale
# applehelp_title
# applehelp_codesign_identity
# applehelp_codesign_flags
# applehelp_indexer_path
# applehelp_codesign_path
# applehelp_disable_external_tools

# -- Options for epub output --------------------------------------------------

# epub_basename
# epub_theme
# epub_theme_options
epub_title = project
# epub_description
epub_author = author
# epub_contributor
# epub_language
epub_publisher = author
epub_copyright = copyright
# epub_identifier
# epub_scheme
# epub_uid
# epub_cover
# epub_guide
# epub_pre_files
# epub_post_files
epub_exclude_files = ['search.html']
# epub_tocdepth
# epub_tocdup
# epub_tocscope
# epub_fix_images
# epub_max_image_width
# epub_show_urls
# epub_use_index
# epub_writing_mode
# epub3_page_progression_direction

# -- Options for LaTeX output -------------------------------------------------

# latex_engine
latex_documents = [(master_doc,                  # startdocname
                    project + '.tex',            # targetname
                    project.replace('_', '\\_') + ' Documentation',  # title
                    author,                      # author
                    'manual',                    # documentclass
                    False),                      # toctree_only
                   ]
# latex_logo
# latex_toplevel_sectioning
# latex_use_parts
# latex_appendices
# latex_domain_indices
# latex_use_modindex
# latex_show_pagerefs
# latex_show_urls
# latex_keep_old_macro_names
# latex_elements
# latex_docclass
# latex_additional_files
# latex_preamble
# latex_paper_size
# latex_font_size

# -- Options for text output --------------------------------------------------

# text_newlines
# text_sectionchars

# -- Options for manual page output -------------------------------------------

man_pages = [(master_doc,                  # startdocname
              project,                     # name
              project + ' Documentation',  # description
              author,                      # authors
              1),                          # section
             ]
# man_show_urls

# -- Options for Texinfo output -----------------------------------------------

texinfo_documents = [(master_doc,                   # startdocname
                      project,                      # targetname
                      project + ' Documentation',   # title
                      author,                       # author
                      project,                      # dir_entry
                      project_description,          # description
                      project_category,             # category
                      False),                       # toctree_only
                     ]
# texinfo_appendices
# texinfo_domain_indices
# texinfo_show_urls
# texinfo_no_detailmenu
# texinfo_elements

# -- Options for the linkcheck builder ----------------------------------------

# linkcheck_ignore
# linkcheck_retries
# linkcheck_timeout
# linkcheck_workers
# linkcheck_anchors
# linkcheck_anchors_ignore

# -- Options for the XML builder ----------------------------------------------

# xml_pretty

# -- Options for the C++ domain -----------------------------------------------

# cpp_index_common_prefix
# cpp_id_attributes
# cpp_paren_attributes

# #############################################################################
# sphinx.ext.autodoc
# #############################################################################

autoclass_content = "both"
# autodoc_member_order
autodoc_default_options = {'members': True, 'inherited-members': True}
autodoc_docstring_signature = True
# autodoc_mock_imports


# #############################################################################
# sphinx.ext.autosummary
# #############################################################################

# Workaround for https://github.com/sphinx-doc/sphinx/issues/6695
# otherwise we could also use = False here.
autosummary_generate = []

# #############################################################################
# sphinx.ext.coverage
# #############################################################################

# coverage_ignore_modules
# coverage_ignore_functions
# coverage_ignore_classes
# coverage_c_path
# coverage_c_regexes
# coverage_ignore_c_items
# coverage_write_headline
# coverage_skip_undoc_in_source

# #############################################################################
# sphinx.ext.doctest
# #############################################################################

# doctest_default_flags
# doctest_path
# doctest_global_setup
# doctest_global_cleanup
# doctest_test_doctest_blocks

# #############################################################################
# sphinx.ext.extlinks
# #############################################################################

# extlinks

# #############################################################################
# sphinx.ext.graphviz
# #############################################################################

# graphviz_dot
# graphviz_dot_args
# graphviz_output_format

# #############################################################################
# sphinx.ext.inheritance_diagram
# #############################################################################

# inheritance_graph_attrs
# inheritance_node_attrs
# inheritance_edge_attrs

# #############################################################################
# sphinx.ext.intersphinx
# #############################################################################

intersphinx_mapping = {'python': ('https://docs.python.org/3.8/',  # target
                                  None),                           # inventory
                       }
# intersphinx_cache_limit
# intersphinx_timeout

# #############################################################################
# sphinx.ext.linkcode
# #############################################################################

# linkcode_resolve

# #############################################################################
# All math
# #############################################################################

# math_number_all

# #############################################################################
# sphinx.ext.imgmath
# #############################################################################

# imgmath_image_format
# imgmath_latex
# imgmath_dvipng
# imgmath_dvisvgm
# imgmath_latex_args
# imgmath_latex_preamble
# imgmath_dvipng_args
# imgmath_dvisvgm_args
# imgmath_use_preview
# imgmath_add_tooltips
# imgmath_font_size

# #############################################################################
# sphinx.ext.mathjax
# #############################################################################

# mathjax_path

# #############################################################################
# sphinx.ext.jsmath
# #############################################################################

# jsmath_path

# #############################################################################
# sphinx.ext.todo
# #############################################################################

# todo_include_todos
# todo_emit_warnings
# todo_link_only

# #############################################################################
# sphinx.ext.napoleon
# #############################################################################

# napoleon_google_docstring
# napoleon_numpy_docstring
# napoleon_include_init_with_doc
# napoleon_include_private_with_doc
# napoleon_include_special_with_doc
# napoleon_use_admonition_for_examples
# napoleon_use_admonition_for_notes
# napoleon_use_admonition_for_references
# napoleon_use_ivar
# napoleon_use_param
# napoleon_use_keyword
# napoleon_use_rtype

# #############################################################################
# numpydoc
# #############################################################################

# numpydoc_use_plots
numpydoc_show_class_members = False
# numpydoc_show_inherited_class_members
# numpydoc_class_members_toctree
