# Generated by Xpand using M2Make template.

# Model of 'build' package.

ifndef __mybuild_build_model_mk
__mybuild_build_model_mk := 1

include mk/model/model_impl.mk

#
# Model object 'Build'.
#
# The following features are defined:
#   - reference 'configuration'
#   - reference 'modules'
#
define class-Build
	# Extends 'EObject' class (implicitly).
	$(eobject Build_Build,
		Build,,)

	# Property 'configuration : CfgConfiguration'.
	# Property 'configuration_link : ELink'.
	$(eobject-reference Build_Build_configuration,
		configuration,CfgConfiguration,,changeable linkable)

	# Property 'modules... : ModuleInstance'.
	$(eobject-reference Build_Build_modules,
		modules,ModuleInstance,build,changeable many containment)

	# PROTECTED REGION ID(Build_Build) ENABLED START
	$(property-field issueReceiver : IssueReceiver)

	$(map moduleInstanceByName : BuildModuleInstance)

	# PROTECTED REGION END
endef

#
# Model object 'ModuleInstance'.
#
# The following features are defined:
#   - reference 'build'
#   - reference 'type'
#   - reference 'allTypes'
#   - reference 'dependent'
#   - reference 'depends'
#   - reference 'contents'
#   - reference 'container'
#   - reference 'options'
#   - reference 'afterDepends'
#   - reference 'includeMember'
#   - reference 'sources'
#
define class-ModuleInstance
	# Extends 'EObject' class (implicitly).
	$(eobject Build_ModuleInstance,
		ModuleInstance,,)

	# Property 'build : Build'.
	$(eobject-reference Build_ModuleInstance_build,
		build,Build,modules,changeable container)

	# Property 'type : MyModuleType'.
	$(eobject-reference Build_ModuleInstance_type,
		type,MyModuleType,,changeable)

	# Reference 'allTypes' [0..*]: derived, read-only.
	$(property allTypes... : MyModuleType)
	# PROTECTED REGION ID(Build_ModuleInstance_allTypes) ENABLED START
	$(getter allTypes,
		$(for t <- $(get type),$t $(get t->allSuperTypes)))
	# PROTECTED REGION END

	# Property 'dependent... : ModuleInstance'.
	$(eobject-reference Build_ModuleInstance_dependent,
		dependent,ModuleInstance,depends,changeable many)

	# Property 'depends... : ModuleInstance'.
	$(eobject-reference Build_ModuleInstance_depends,
		depends,ModuleInstance,dependent,changeable many)

	# Property 'contents... : ModuleInstance'.
	$(eobject-reference Build_ModuleInstance_contents,
		contents,ModuleInstance,container,changeable many)

	# Property 'container : ModuleInstance'.
	$(eobject-reference Build_ModuleInstance_container,
		container,ModuleInstance,contents,changeable)

	# Property 'options... : OptionInstance'.
	$(eobject-reference Build_ModuleInstance_options,
		options,OptionInstance,module,changeable many containment)

	# Property 'afterDepends... : ModuleInstance'.
	$(eobject-reference Build_ModuleInstance_afterDepends,
		afterDepends,ModuleInstance,,changeable many)

	# Property 'includeMember : CfgInclude'.
	# Property 'includeMember_link : ELink'.
	$(eobject-reference Build_ModuleInstance_includeMember,
		includeMember,CfgInclude,,changeable linkable)

	# Reference 'sources' [0..*]: derived, read-only.
	$(property sources... : MyFileName)
	# PROTECTED REGION ID(Build_ModuleInstance_sources) ENABLED START
	$(getter sources,
		$(for t <- $(get allTypes),
			src <- $(get t->sources),
			$(get src->fileFullName)))
	# PROTECTED REGION END

	# PROTECTED REGION ID(Build_ModuleInstance) ENABLED START
	# PROTECTED REGION END
endef

#
# Model object 'OptionInstance'.
#
# The following features are defined:
#   - reference 'module'
#   - reference 'option'
#   - reference 'value'
#
define class-OptionInstance
	# Extends 'EObject' class (implicitly).
	$(eobject Build_OptionInstance,
		OptionInstance,,)

	# Property 'module : ModuleInstance'.
	$(eobject-reference Build_OptionInstance_module,
		module,ModuleInstance,options,changeable container)

	# Property 'option : MyOption'.
	# Property 'option_link : ELink'.
	$(eobject-reference Build_OptionInstance_option,
		option,MyOption,,changeable linkable)

	# Property 'value : MyLiteral'.
	$(eobject-reference Build_OptionInstance_value,
		value,MyLiteral,,changeable)

	# PROTECTED REGION ID(Build_OptionInstance) ENABLED START
#	# TODO Add custom implementation here and remove this comment.
	# PROTECTED REGION END
endef

# PROTECTED REGION ID(Build) ENABLED START
# TODO Add custom implementation here and remove this comment.
# PROTECTED REGION END

$(def_all)

endif # __mybuild_build_model_mk

